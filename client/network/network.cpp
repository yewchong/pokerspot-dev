/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : network.cpp
  Created  : 25.10.1999

  OVERVIEW : Basic networking functionality.

 ****************************************************************************/

#include "stdafx.h"
#include "network/errorlog.h"

#ifdef PCLIENT_MODULE_
  #include "ui/cmdid.h"
  #include "ui/chatview.h"  
  #ifdef _DEBUG
    #define new DEBUG_NEW
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
  #endif
#else
  #define ID_PDURECEIVED 101
#endif

#include "base/sync.h"
#include "network/network.h"
#include "network/pdunotify.h"
#include "network/pduchat.h"
#include "network/pduping.h"
#include "network/pduplayerinfo.h"
#include "network/pduplayer.h"
#include "network/pduallin.h"
#include "network/pduanteallin.h"
#include "network/pdubutton.h"
#include "network/pdugamenumber.h"
#include "network/pdusetup.h"
#include "network/pduupgraderequest.h"
#include "network/pdudealcards.h"
#include "network/pdudealcardsflex.h"
#include "network/pdublindcard.h"
#include "network/pdupocketcard.h"
#include "network/pducommunitycard.h"
#include "network/pduactionrequest.h"
#include "network/pduactionrequestspreadlimit.h"
#include "network/pduaction.h"
#include "network/pduactionecho.h"
#include "network/pdublindrequest.h"
#include "network/pdupostblind.h"
#include "network/pdusitinquery.h"
#include "network/pdusitin.h"
#include "network/pdusitout.h"
#include "network/pdushowcardsquery.h"
#include "network/pdushowcards.h"
#include "network/pdushowdown.h"
#include "network/pduannounce.h"
#include "network/pdutournamentparams.h"
#include "network/pduplayerfinished.h"
#include "network/pdureseat.h"
#include "network/pduchatenable.h"
#ifndef PSPOT_TABLE_MODULE_
#include "network/pduaccountbalance.h"
#include "network/pduverbosetablereply.h"
#include "network/pdutournamentdetailsreply.h"
#include "network/pduloginstats.h"
#include "network/pduqueueupdate.h"
#include "network/pduqueuefull.h"
#include "network/pducreditcardresponse.h"
#include "network/pducreditcardreply2.h"
#include "network/pducreditcardtypes.h"
#include "network/pduloungebuyinresponse.h"
#include "network/pduloungecashoutresponse.h"
#include "network/pduloungetransactionreply.h"
#include "network/pdulounge.h"
#include "network/pduloungegamenames.h"
#include "network/pduredirectlogin.h"
#endif
#include "network/pdunetworkerror.h"
#include "network/pduerror.h"
#include "network/ssl.h"
#include <stdio.h>
#include <stdlib.h>
#include <list>

    
BEGIN_NAMESPACE(Network)


#ifdef _DEBUG
DWORD PDUHeader::AllocBalance_ = 0;
#endif

PDUHeader* AllocPDU(const char* buf);

// 
// Module glogals
//

// 64 kB is maximum size for a PDU.
//char g_ReadBuffer[PDU_MAX_SIZE];
char g_WriteBuffer[PDU_MAX_SIZE];

// 
// Extract one PDU from a socket.
//
// NOTE: the returned PDU is allocated dynamically.
// Caller is responsible for deallocation.
//
// On failure, returns SOCKET_ERROR or 0.
// Caller can call WSAGetLastError to get the
// actual error code.
//
int ReadPDU(SOCKET s, char* readBuf, PDUHeader** ppPDU)
{
  char* bptr = readBuf;
  int n = 0, buflen = PDU_HEADER_SIZE;

  //
  // Read the PDU header
  //
  while (buflen > 0 &&
         (n = PSI_SSL_Read(s, bptr, buflen)) != SOCKET_ERROR &&
          n > 0)
  {
    bptr += n;
    buflen -= n;
  }

  if (n == SOCKET_ERROR) return n;
  
  PDUHeader pduh(readBuf);
  buflen = pduh.length_ - PDU_HEADER_SIZE;
  //
  // Read PDU specific extra bytes if any
  //
  while (buflen > 0 && 
         (n = PSI_SSL_Read(s, bptr, buflen)) != SOCKET_ERROR &&
          n > 0)
  {
    bptr += n;
    buflen -= n;
  }

  if (n != SOCKET_ERROR && n > 0)
    *ppPDU = AllocPDU(readBuf);

  if (*ppPDU)
    (*ppPDU)->checkValid();

  return n;
}

//
// Write one PDU to socket.
//
int SendPDU(SOCKET s, const PDUHeader& pdu)
{
  pdu.write(g_WriteBuffer);
  return PSI_SSL_Write(s, g_WriteBuffer, pdu.length_);

#ifdef SERIALIZE_IMPLEMENTED_
  // The serialize stuff is not yet fully implemented
  WriteBuffer buf(g_WriteBuffer);
  pdu.serialize(buf);
  ASSERT(buf.getsize() == pdu.length_);
  return PSI_SSL_Write(s, buf.getbuf(), pdu.length_, 0);
#endif
}


int Send(SOCKET s, const char* buf, size_t nbytes)
{
  return send(s, buf, nbytes, 0);
}

struct RAIIRunning
{
    RAIIRunning(ThreadInfo* pti)
        : pti_(pti)
    { pti_->bRunning = TRUE; }
    ~RAIIRunning()
    { pti_->bRunning = FALSE; }
    ThreadInfo* pti_;
};

struct RAIIBuf
{
    RAIIBuf(char* buf)
      : buf_(buf) { }
    ~RAIIBuf()
    { delete [] buf_; }
    char* buf_;
};


#ifdef PCLIENT_MODULE_
//
// This is the Socket Reader Thread function.
//  reading PDUs out of
// the socket. Upon receiving a PDU, a message is
// posted to the main UI thread along with the PDU
// object as a parameter. The main UI thread
// handles the message and eventually deallocates
// the PDU object.
//
// The function loops forever
//
// This strategy has advantages:
// - No CPU time is wasted on polling the socket,
//   instead, the socket reader thread sleeps if
//   there are no incoming PDUs.
// - PostMessage takes care of synchronizing
//   between the socket reader thread and the
//   main UI thread.
// - The main UI thread sees incoming PDUs as
//   ordinary Windows messages.
//
UINT SocketReaderProc(VOID* pV)
{
  ThreadInfo* pTI = reinterpret_cast<ThreadInfo*>(pV);

  RAIIRunning r(pTI);
  RAIIBuf buf = new char[PDU_MAX_SIZE];
  if (!buf.buf_)
  {
    AfxMessageBox("Out of memory!");
    return -1;
  }

  CWinThread* winThread = AfxGetThread();
  ASSERT(winThread);
  DWORD threadID = winThread->m_nThreadID;

  while (!pTI->bQuit && ::IsWindow(pTI->hWnd))
  {
    PDUHeader* pPDU = 0;
    
    int bytesRead = ReadPDU(pTI->s, buf.buf_, &pPDU);

    if (bytesRead == 0 && pPDU == NULL)
        break;

    if (bytesRead != SOCKET_ERROR && bytesRead > 0)
    {
      if (pPDU)
      { 
        // Save thread id
        //pPDU->setThreadID(threadID);

        // Post it to the main UI thread
        ::PostMessage(pTI->hWnd,
                      WM_COMMAND,
                      MAKEWPARAM(ID_PDURECEIVED, pPDU->type_),
                      reinterpret_cast<LPARAM>(pPDU));
      }
    }
    else if (bytesRead == SOCKET_ERROR && WSAGetLastError() != 0)
    { 
      if (!pTI->bQuit)
      {
          CString s;
          s.Format("Read error(1): %i", WSAGetLastError());
          LogError(s);
      }

      // A network error has occurred -
      // post an error PDU and exit
      if (!pTI->bQuit && ::IsWindow(pTI->hWnd))
      {
        pPDU = new PDUNetworkError(WSAGetLastError());
        ::PostMessage(pTI->hWnd,
                      WM_COMMAND,
                      MAKEWPARAM(ID_PDURECEIVED, pPDU->type_),
                      reinterpret_cast<LPARAM>(pPDU));
      }
      return -1;
    }
    else
    { // SSL makes the read sometimes return -1
      // even if there is no real error. The next
      // read will succeed.
      /*
      CString s;
      s.Format("Read error(2): %i %i", bytesRead, WSAGetLastError());
      LogError(s);
      */
    }
  }

  return 0;
}
#endif

//
// PDU allocator functions, one for each PDU type.
//

#ifndef PSPOT_TABLE_MODULE_
PDUHeader* AllocPDU_Lounge_LoginStats(const char* buf)
{
  return new PDULoungeLoginStats(buf);
}

PDUHeader* AllocPDU_Lounge_Accept(const char* buf)
{
  return new PDULoungeAccept(buf);
}

PDUHeader* AllocPDU_Lounge_Reject(const char* buf)
{
  return new PDULoungeReject(buf);
}

PDUHeader* AllocPDU_Lounge_Survey(const char* buf)
{
  return new PDULoungeSurvey(buf);
}

PDUHeader* AllocPDU_Lounge_Status(const char* buf)
{
  return new PDULoungeStatus(buf);
}

PDUHeader* AllocPDU_Lounge_GameNames(const char* buf)
{
  return new PDUGameNames(buf);
}

PDUHeader* AllocPDU_Lounge_TableFull(const char* buf)
{
  return new PDULoungeTableFull(buf);
}

PDUHeader* AllocPDU_Lounge_Sitting(const char* buf)
{
  return new PDULoungeSitting(buf);
}

PDUHeader* AllocPDU_Lounge_Observing(const char* buf)
{
  return new PDULoungeObserving(buf);
}

PDUHeader* AllocPDU_Lounge_Logout(const char* buf)
{
  return new PDULoungeLogout(buf);
}

PDUHeader* AllocPDU_Lounge_LogoutWarning(const char* buf)
{
  return new PDULoungeLogoutWarning(buf);
}

PDUHeader* AllocPDU_Lounge_LogoutCertain(const char* buf)
{
  return new PDULoungeLogoutCertain(buf);
}

PDUHeader* AllocPDU_Lounge_LogoutConfirmed(const char* buf)
{
  return new PDULoungeLogoutConfirmed(buf);
}

PDUHeader* AllocPDU_Lounge_TableLogout(const char* buf)
{
  return new PDULoungeTableLogout(buf);
}

PDUHeader* AllocPDU_Lounge_AccountBalance(const char* buf)
{
  return new PDUAccountBalance(buf);
}

PDUHeader* AllocPDU_Lounge_QueueUpdate(const char* buf)
{
  return new PDUQueueUpdate2(buf);
}

PDUHeader* AllocPDU_Lounge_QueueFull(const char* buf)
{
  return new PDUQueueFull(buf);
}

PDUHeader* AllocPDU_Lounge_CreditCardReply(const char* buf)
{
  return new PDUCreditCardReply(buf);
}

PDUHeader* AllocPDU_Lounge_CreditCardReply2(const char* buf)
{
  return new PDUCreditCardReply2(buf);
}

PDUHeader* AllocPDU_Lounge_CreditCardTypes(const char* buf)
{
  return new PDUCreditCardTypes(buf);
}

PDUHeader* AllocPDU_ChatEnable(const char* buf)
{
  return new PDUEnableChat(buf);
}

PDUHeader* AllocPDU_Lounge_BuyInReply(const char* buf)
{
  return new PDULoungeBuyInReply(buf);
}

PDUHeader* AllocPDU_Lounge_CashOutReply(const char* buf)
{
  return new PDULoungeCashOutReply(buf);
}

PDUHeader* AllocPDU_Lounge_TransactionReply(const char* buf)
{
  return new PDULoungeTransactionReply(buf);
}

PDUHeader* AllocPDU_Lounge_VerboseTableReply(const char* buf)
{
  return new PDUVerboseTableReply(buf);
}

PDUHeader* AllocPDU_LB_RedirectLogin(const char* buf)
{
  return new CPDURedirectLogin(buf);
}


PDUHeader* AllocPDU_Tournament_DetailsReply(const char* buf)
{
  return new PDUTournamentDetailsReply(buf);
}


#endif

PDUHeader* AllocPDU_Setup_TableLogin(const char* buf)
{
  return new PDUSetupTableLogin(buf);
}

PDUHeader* AllocPDU_Setup_TableAccept(const char* buf)
{
  return new PDUSetupTableAccept(buf);
}

PDUHeader* AllocPDU_Setup_TableReject(const char* buf)
{
  return new PDUSetupTableReject(buf);
}

PDUHeader* AllocPDU_Setup_BuyInQuery(const char* buf)
{
  return new PDUSetupBuyInQuery(buf);
}

PDUHeader* AllocPDU_Setup_BuyInRequest(const char* buf)
{
  return new PDUSetupBuyInRequest(buf);
}

PDUHeader* AllocPDU_Setup_TableLogout(const char* buf)
{
  return new PDUSetupTableLogout(buf);
}

PDUHeader* AllocPDU_Setup_TableLogoutConfirm(const char* buf)
{
  return new PDUSetupTableLogoutConfirm(buf);
}

PDUHeader* AllocPDU_Play_Button(const char* buf)
{
  return new PDUButton(buf);
}

PDUHeader* AllocPDU_Play_BlindRequest(const char* buf)
{
  return new PDUBlindRequest(buf);
}

PDUHeader* AllocPDU_Play_PostBlind(const char* buf)
{
  return new PDUPostBlind(buf);
}

PDUHeader* AllocPDU_Play_SitInQuery(const char* buf)
{
  return new PDUSitInQuery(buf);
}

PDUHeader* AllocPDU_Play_SitIn(const char* buf)
{
  return new PDUSitIn(buf);
}

PDUHeader* AllocPDU_Play_SitOut(const char* buf)
{
  return new PDUSitOut(buf);
}

PDUHeader* AllocPDU_Play_ShowCardsQuery(const char* buf)
{
  return new PDUShowCardsQuery(buf);
}

PDUHeader* AllocPDU_Play_ShowCards(const char* buf)
{
  return new PDUShowCards(buf);
}

PDUHeader* AllocPDU_Play_CommunityCard(const char* buf)
{
  return new PDUCommunityCard(buf);
}

PDUHeader* AllocPDU_Play_BlindCard(const char* buf)
{
  return new PDUBlindCard(buf);
}

PDUHeader* AllocPDU_Play_PocketCard(const char* buf)
{
  return new PDUPocketCard(buf);
}

PDUHeader* AllocPDU_Play_DealCards(const char* buf)
{
  return new PDUDealCards(buf);
}

PDUHeader* AllocPDU_Play_DealCardsFlex(const char* buf)
{
  return new PDUDealCardsFlex(buf);
}

PDUHeader* AllocPDU_Play_ActionRequest(const char* buf)
{
  return new PDUActionRequest(buf);
}

PDUHeader* AllocPDU_Play_ActionRequestSpreadLimit(const char* buf)
{
  return new PDUActionRequestSpreadLimit(buf);
}

PDUHeader* AllocPDU_Play_Action(const char* buf)
{
  return new PDUAction(buf);
}

PDUHeader* AllocPDU_Play_ActionEcho(const char* buf)
{
  return new PDUActionEcho(buf);
}

PDUHeader* AllocPDU_Play_Showdown(const char* buf)
{
  return new PDUShowdown(buf);
}

PDUHeader* AllocPDU_Play_Announce(const char* buf)
{ 
  return new PDUAnnounce(buf);
}

PDUHeader* AllocPDU_Tournament_Params(const char* buf)
{ 
  return new PDUTournamentParams(buf);
}

PDUHeader* AllocPDU_Tournament_PlayerFinished(const char* buf)
{ 
  return new PDUTournamentPlayerFinished(buf);
}

PDUHeader* AllocPDU_Tournament_ReseatPlayer(const char* buf)
{ 
  return new PDUReseat(buf);
}

PDUHeader* AllocPDU_Chat(const char* buf)
{
  return new PDUChat(buf);
}

PDUHeader* AllocPDU_Server_Notify(const char* buf)
{
  return new PDUNotify(buf);
}

PDUHeader* AllocPDU_UpgradeRequest(const char* buf)
{
  return new PDUUpgradeRequest(buf);
}

PDUHeader* AllocPDU_Ping(const char* buf)
{
  return new PDUPing(buf);
}

PDUHeader* AllocPDU_GameNumber(const char* buf)
{
  return new PDUGameNumber(buf);
}

PDUHeader* AllocPDU_Setup_PlayerInfo(const char* buf)
{
  return new PDUPlayerInfo(buf);
}

PDUHeader* AllocPDU_Player(const char* buf)
{
  return new PDUPlayer(buf);
}

PDUHeader* AllocPDU_Play_AllIn(const char* buf)
{
  return new PDUAllIn(buf);
}

PDUHeader* AllocPDU_Tournament_AnteAllIn(const char* buf)
{
  return new PDUAnteAllIn(buf);
}

PDUHeader* AllocPDU_Error_Client(const char* buf)
{
  return new PDUErrorClient(buf);
}

PDUHeader* AllocPDU_Error_Server(const char* buf)
{
  return new PDUErrorServer(buf);
}

//
// Make a global array that binds a PDU type number
// with the corresponding allocator function. In other
// words, the PDU type is used as an index to this array;
// the i'th entry in the array can allocate a PDU
// of type i.
//
#define PDU_ENTRY(x) { x, #x, Alloc##x }

struct PDUAllocator
{
  PDU_Type     type_;
  LPCSTR       name_;
  PDUAllocFunc pAllocFunc_;
} g_PDUAllocators[] =
{
#ifdef PSPOT_TABLE_MODULE_
  // The most time-critical PDUs (i.e., gameplay
  // pdus) come first - consider replacing this with
  // a hash table!!!
  PDU_ENTRY(PDU_Play_ActionRequest),
  PDU_ENTRY(PDU_Play_ActionRequestSpreadLimit),
  PDU_ENTRY(PDU_Play_Action),
  PDU_ENTRY(PDU_Play_ActionEcho),
  PDU_ENTRY(PDU_Play_Button),
  PDU_ENTRY(PDU_Play_BlindRequest),
  PDU_ENTRY(PDU_Play_PostBlind),
  PDU_ENTRY(PDU_Play_SitInQuery),
  PDU_ENTRY(PDU_Play_SitIn),
  PDU_ENTRY(PDU_Play_SitOut),
  PDU_ENTRY(PDU_Play_ShowCardsQuery),
  PDU_ENTRY(PDU_Play_ShowCards),
  PDU_ENTRY(PDU_Play_CommunityCard),
  PDU_ENTRY(PDU_Play_BlindCard),
  PDU_ENTRY(PDU_Play_PocketCard),
  PDU_ENTRY(PDU_Play_DealCards),
  PDU_ENTRY(PDU_Play_DealCardsFlex),
  PDU_ENTRY(PDU_Play_Showdown),
  PDU_ENTRY(PDU_Play_Announce),
  PDU_ENTRY(PDU_Play_AllIn),
  PDU_ENTRY(PDU_Tournament_AnteAllIn),
  PDU_ENTRY(PDU_GameNumber),
  PDU_ENTRY(PDU_Player),

  PDU_ENTRY(PDU_Setup_PlayerInfo),
  PDU_ENTRY(PDU_Setup_TableLogin),
  PDU_ENTRY(PDU_Setup_TableAccept),
  PDU_ENTRY(PDU_Setup_TableReject),
  PDU_ENTRY(PDU_Setup_BuyInQuery),
  PDU_ENTRY(PDU_Setup_BuyInRequest),
  PDU_ENTRY(PDU_Setup_TableLogout),
  PDU_ENTRY(PDU_Setup_TableLogoutConfirm),
  PDU_ENTRY(PDU_Tournament_Params),
  PDU_ENTRY(PDU_Tournament_ReseatPlayer),
  PDU_ENTRY(PDU_Tournament_PlayerFinished),
#endif

#ifndef PSPOT_TABLE_MODULE_
  PDU_ENTRY(PDU_Lounge_Accept),
  PDU_ENTRY(PDU_Lounge_Reject),
  PDU_ENTRY(PDU_Lounge_Survey),
  PDU_ENTRY(PDU_Lounge_Status),
  PDU_ENTRY(PDU_Lounge_GameNames),
  PDU_ENTRY(PDU_Lounge_TableFull),
  PDU_ENTRY(PDU_Lounge_Sitting),
  PDU_ENTRY(PDU_Lounge_Logout),
  PDU_ENTRY(PDU_Lounge_LogoutWarning),
  PDU_ENTRY(PDU_Lounge_LogoutCertain),
  PDU_ENTRY(PDU_Lounge_LogoutConfirmed),
  PDU_ENTRY(PDU_Lounge_TableLogout),
  PDU_ENTRY(PDU_Lounge_LoginStats),
  PDU_ENTRY(PDU_Lounge_VerboseTableReply),
  PDU_ENTRY(PDU_Tournament_DetailsReply),
  PDU_ENTRY(PDU_Lounge_CreditCardReply),
  PDU_ENTRY(PDU_Lounge_CreditCardReply2),
  PDU_ENTRY(PDU_Lounge_CreditCardTypes),
  PDU_ENTRY(PDU_ChatEnable),
  PDU_ENTRY(PDU_Lounge_BuyInReply),
  PDU_ENTRY(PDU_Lounge_CashOutReply),
  PDU_ENTRY(PDU_Lounge_QueueUpdate),
  PDU_ENTRY(PDU_Lounge_QueueFull),
  PDU_ENTRY(PDU_Lounge_AccountBalance),
  PDU_ENTRY(PDU_Lounge_TransactionReply),
  PDU_ENTRY(PDU_Lounge_Observing),
  PDU_ENTRY(PDU_LB_RedirectLogin),
#endif

  PDU_ENTRY(PDU_Chat),
  PDU_ENTRY(PDU_Ping),
  PDU_ENTRY(PDU_UpgradeRequest),

  PDU_ENTRY(PDU_Error_Client),
  PDU_ENTRY(PDU_Error_Server),
  PDU_ENTRY(PDU_Server_Notify)
};


//
// Return the name of pdu type 't' as
// a string.
//
LPCSTR GetName(PDU_Type t)
{
  for (unsigned int i = 0;
       i < (sizeof(g_PDUAllocators) / sizeof(g_PDUAllocators[0]));
       i++)
  {
    if (g_PDUAllocators[i].type_ == t)
      return g_PDUAllocators[i].name_;
  }

  static LPCSTR unknown = "Unknown";
  return unknown;
}

//
// Extract the PDU type from buffer 'buf' and
// allocate and return a PDU of that type.
//
PDUHeader* AllocPDU(const char* buf)
{
  PDU_Type t = (PDU_Type)ntohs(*(u_int16_t*)buf);

  for (unsigned int i = 0;
       i < (sizeof(g_PDUAllocators) / sizeof(g_PDUAllocators[0]));
       i++)
  {
    if (g_PDUAllocators[i].type_ == t)
      return (g_PDUAllocators[i].pAllocFunc_)(buf);
  }

  PDU_DEBUG(  "****************************");
  char b[1024];
  sprintf(b, "*** Unknown PDU type: %d, will be ignored", t);
  PDU_DEBUG(b);
  PDU_DEBUG(  "****************************");
  OutputDebugString(b);
  OutputDebugString("\n");

  return 0;
}

END_NAMESPACE(Network)


