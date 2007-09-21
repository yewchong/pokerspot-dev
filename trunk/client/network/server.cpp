/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Server
  Created  : 4.11.1999

  OVERVIEW : Networking support for Texas Holdem. Class server is a kind
             of a proxy: it represents the PokerSpot server as if it was
             in-process.

 ****************************************************************************/

#include "stdafx.h"
#include "network/server.h"
#include "network/pdulounge.h"
#include "network/pdusetup.h"
#include "network/pduobserverlogin.h"
#include "network/pduobserverlogout.h"
#include "network/pdubuyinmore.h"
#include "network/pduchat.h"
#include "network/pduping.h"
#include "network/pduplayer.h"
#include "network/pduaction.h"
#include "network/pduactionspreadlimit.h"
#include "network/pdusitin.h"
#include "network/pdusitout.h"
#include "network/pdupostblind.h"
#include "network/pdurefuseblind.h"
#include "network/pdushowcards.h"
#include "network/pdumoduleversion.h"
#include "network/pduverbosetablequery.h"
#include "network/pdutournamentdetailsquery.h"
#include "network/pducreditcardquery.h"
#include "network/pducreditcardquery2.h"
#include "network/pducreditcardedit.h"
#include "network/pdubalancequery.h"
#include "network/pduloungebuyinrequest.h"
#include "network/pduloungecashoutrequest.h"
#include "network/pduloungetransactionquery.h"
#include "network/pduhandlogrequest.h"
#include "network/pdueditrules.h"
#include "network/pduheartbeat.h"
#include "network/ssl.h"
#include "ui/mainwnd.h"
#include "ui/global.h"
#ifdef PSPOT_TABLE_MODULE_
  #include "ui/tableview.h"
  #include "ui/cmdid.h"
#else
  #include "network/pduloungehandshake.h"
#endif
#include "base/md5.h"


namespace
{ 

LPCTSTR g_szErrFmt = _T("Unable to connect to host '%s' at port '%d'\n(%s)");
LPCTSTR g_szErrFmtUINT = _T("Unable to connect to host '%d' at port '%d'\n(%s)");


using namespace Network;

CWinThread* StartSocketReaderThread(ThreadInfo* pTI)
{
  return AfxBeginThread(SocketReaderProc, pTI);
}


BOOL MakeSockAddr(const char* host, short port, struct sockaddr* pResult)
{
  ASSERT(host != NULL && *host != '\0');

  struct hostent* pHostEnt = 0;
  SOCKADDR_IN     address;

  memset(pResult, 0, sizeof(*pResult));
  memset(&address, 0, sizeof(address));

  pHostEnt = gethostbyname(host);

  if (!pHostEnt)
  { // try with numeric address
    u_long res = inet_addr(host);
    if (res != INADDR_NONE)
      pHostEnt = gethostbyaddr((char*)&res, 4, PF_INET);
  }

  if (pHostEnt != NULL)
  {
    address.sin_family = AF_INET;
    address.sin_port   = port;
    memcpy(&address.sin_addr, pHostEnt->h_addr_list[0], pHostEnt->h_length);
    memcpy(pResult, &address, sizeof(address));
  }
  else
  {
    TRACE1("Error: %d\n", WSAGetLastError());
  }

  return pHostEnt != NULL;
}  


int SendClientConfiguration(SOCKET s,
                                u_byte_t* checksum)
{
  int rc = 0;

  size_t outlength = 16 * sizeof(u_byte_t);
  rc = Send(s, (char*)checksum, outlength);

  if (rc != SOCKET_ERROR)
    return S_OK;
  else
    return E_FAIL;
}

SOCKET connectsock(u_long addr, u_short port)
{
    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));

    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    sin.sin_addr.s_addr = htonl(addr);

    int s = socket(PF_INET, SOCK_STREAM, 0);

    if (s != INVALID_SOCKET)
    {
        int c = connect(s, (struct sockaddr*)&sin, sizeof(sin));
        if (c >= 0)
        {
            BOOL rc = PSI_SSL_Start(s);
            if (!rc)
            {
                TableServer::ErrFmt_ = "can't init SSL (%i)";
                TableServer::ErrCode_ = WSAGetLastError();
                closesocket(s);
            }
            else
            {
                // SUCCESS!
                TableServer::ErrFmt_ = "success: %i";
                TableServer::ErrCode_ = 0;
            }
        }
        else
        {
            TableServer::ErrFmt_ = "connect: %i";
            TableServer::ErrCode_ = WSAGetLastError();
            closesocket(s);
            s = INVALID_SOCKET;
        }
    }
    
    return s;
}


} // namespace



namespace Network
{

//
// Server
//

CString             Server::ErrFmt_;
int                 Server::ErrCode_;
Network::ThreadInfo Server::ThreadInfo_;

CString Server::Username_;
CString Server::Password_;


Server::Server(SOCKET s)
  :
  socket_(s)
{
  ThreadInfo_.s = INVALID_SOCKET;
  ThreadInfo_.hWnd = NULL;
  ThreadInfo_.bQuit = TRUE;
}


Server::~Server()
{
  Quit();
  closeConnection();
}


//
// Server entry point - connect to specified host
// and instantiate a Server object.
//
Server* Server::Connect(HWND hWnd, const char* host, short port)
{
  Server* pServer = 0;

  SOCKET s = MakeConnection(host, htons(port));
  if (s == INVALID_SOCKET)
  {
    CString s, msg;
    s.Format(ErrFmt_, ErrCode_);
    msg.Format(g_szErrFmt, host, port, s);
    AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
  }
  else
  {
    pServer = new Server(s);

    // Note: when the active view changes, 
    // info.hWnd must be changed as well!
    ThreadInfo_.hWnd = hWnd;
    ThreadInfo_.bQuit = FALSE;      
    ThreadInfo_.bRunning = FALSE;      
    ThreadInfo_.s = s;
    pServer->pSocketReader_ = StartSocketReaderThread(&ThreadInfo_);
  }

  return pServer;
}


void Server::closeConnection()
{
  if (socket_ != INVALID_SOCKET)
  {
    PSI_SSL_Close(socket_);
    closesocket(socket_);
  }
  socket_ = INVALID_SOCKET;  
}


int Server::sendPDU(PDUHeader* pPDU)
{
  return 1;
}


void Server::Quit()
{
  if (!ThreadInfo_.bQuit)
  {
    ThreadInfo_.bQuit = TRUE;
    ThreadInfo_.hWnd = NULL;

    if (ThreadInfo_.s != INVALID_SOCKET)
        closesocket(ThreadInfo_.s);
    ThreadInfo_.s = INVALID_SOCKET;

    int loopGuard = 10;
    do
    {
        // Wait for the thread to finish!
        Sleep(500);
    }
    while (ThreadInfo_.bRunning && --loopGuard);
  }
}


int Server::sendChatPDU(const CString& msg)
{
  int rc = 1;

  if (Username_.GetLength() > 0 &&
      Password_.GetLength() > 0)
  {
    PDUChat pdu(Username_, Password_, msg, msg.GetLength());
    rc = Network::SendPDU(socket_, pdu);
  }

  return rc;
}


int Server::sendPongPDU(u_int32_t data)
{
  int rc = 1;

  PDUPong pdu(Username_, Password_, data);
  rc = Network::SendPDU(socket_, pdu);

  return rc;
}


SOCKET Server::MakeConnection(const char* host, short port)
{
  SOCKET s = INVALID_SOCKET;
  struct sockaddr sockAddr;

  if (MakeSockAddr(host, port, &sockAddr))
  {    
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s != INVALID_SOCKET)
    {
      int c = connect(s, &sockAddr, sizeof(sockAddr));
      if (c >= 0)
      {
        BOOL rc = PSI_SSL_Start(s);
        if (!rc)
        {
          ErrFmt_ = "can't init SSL (%i)";
          ErrCode_ = WSAGetLastError();
        }
        else
        {
          // SUCCESS!
          ErrFmt_ = "success: %i";
          ErrCode_ = 0;
        }
      }
      else
      {
        ErrFmt_ = "connect: %i";
        ErrCode_ = WSAGetLastError();
        closesocket(s);
        s = INVALID_SOCKET;
      }
    }
    else
    {
      ErrFmt_ = "socket: %i";
      ErrCode_ = WSAGetLastError();
    }
  }
  else
  {
    ErrFmt_ = "Can't resolve host address: %i";
    ErrCode_ = WSAGetLastError();
    TRACE2("Can't map host name %s port %i\n", host, port);
  }

  return s;
}  


SOCKET Server::MakeConnection(UINT ipaddr, short port)
{
  SOCKET s = INVALID_SOCKET;
  struct sockaddr sockAddr;
  
  IN_ADDR in_addr;
  in_addr.S_un.S_addr = ipaddr;
  char* p = inet_ntoa(in_addr);
  if (p == NULL)
    return INVALID_SOCKET;
  CString host = p;

  if (host == "127.0.0.1")
  { // special case for testing locally
    host = "localhost";
  }

  if (MakeSockAddr((LPCSTR)host, port, &sockAddr))
  {    
    s = socket(AF_INET, SOCK_STREAM, 0);
    if (s != INVALID_SOCKET)
    {
      int c = connect(s, &sockAddr, sizeof(sockAddr));
      if (c >= 0)
      { 
        BOOL rc = PSI_SSL_Start(s);
        if (!rc)
        {
          ErrFmt_ = "can't init SSL (%i)";
          ErrCode_ = WSAGetLastError();
        }
        else
        {
          // SUCCESS!
          ErrFmt_ = "success: %i";
          ErrCode_ = 0;
        }
      }
      else
      {
        ErrFmt_ = "connect: %i";
        ErrCode_ = WSAGetLastError();
        closesocket(s);
        s = INVALID_SOCKET;
      }
    }
    else
    {
      ErrFmt_ = "socket: %i";
      ErrCode_ = WSAGetLastError();
    }
  }
  else
  {
    ErrFmt_ = "Can't resolve host address: %i";
    ErrCode_ = WSAGetLastError();
    TRACE2("Can't map host name %s port %i\n", host, port);
  }

  return s;
}  



//
// Lounge Server implementation
//

LoungeServer::LoungeServer(SOCKET s)
  :
  Server(s)
{}


LoungeServer* LoungeServer::ConnectToLounge(HWND hWnd,
                                            const char* host,
                                            short port)
{
  LoungeServer* pServer = 0;

#ifndef PSPOT_TABLE_MODULE_

  SOCKET s = MakeConnection(host, htons(port));
  if (s == INVALID_SOCKET)
  {
    CString s, msg;
    s.Format(ErrFmt_, ErrCode_);
    msg.Format(g_szErrFmt, host, port, s);
    AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
  }
  else
  {
    pServer = new LoungeServer(s);

    // Note: when the active view changes, 
    // info.hWnd must be changed as well!
    ThreadInfo_.hWnd = hWnd;
    ThreadInfo_.bQuit = FALSE;      
    ThreadInfo_.bRunning = FALSE;      
    ThreadInfo_.s = s;
    pServer->pSocketReader_ = StartSocketReaderThread(&ThreadInfo_);
  }

#endif

  return pServer;
}

LoungeServer* LoungeServer::ConnectToLounge(HWND hWnd,
                                            ULONG hostIpAddress,
                                            short port)
{
  LoungeServer* pServer = 0;

#ifndef PSPOT_TABLE_MODULE_

  SOCKET s = connectsock(hostIpAddress, port);
  if (s == INVALID_SOCKET)
  {
    CString s, msg;
    s.Format(ErrFmt_, ErrCode_);
    msg.Format(g_szErrFmtUINT, hostIpAddress, port, s);
    AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
  }
  else
  {
    pServer = new LoungeServer(s);

    // Note: when the active view changes, 
    // info.hWnd must be changed as well!
    ThreadInfo_.hWnd = hWnd;
    ThreadInfo_.bQuit = FALSE;      
    ThreadInfo_.bRunning = FALSE;      
    ThreadInfo_.s = s;
    pServer->pSocketReader_ = StartSocketReaderThread(&ThreadInfo_);
  }

#endif

  return pServer;
}


int LoungeServer::sendLoungeHandshake(const char* username)
{
  int rc = 1;

#ifndef PSPOT_TABLE_MODULE_

  // First prepare the client configuration data
  u_byte_t checksum[16];
  memset(checksum, 0, sizeof(checksum));
  if (Base::ComputeMD5Checksum("client_versions.conf",
                               checksum) == -1)
  {
    // Cannot open the configuration file!!!
    // Will send 0 as the checksum and the new file
    // will automatically downloaded.
    AfxMessageBox("The client configuration file 'client_versions.conf' was not "
                  "found. This file must be located in the PokerSpot client installation directory.");
    return 0;
  }

  CPDULoungeHandshake pdu(username,
                          0, // platform == WIN32
                          checksum);

  rc = Network::SendPDU(socket_, pdu);
#endif

  return rc;
}


int LoungeServer::sendLoungeLoginPDU(const char* username,
                                     const char* password)
{
  int rc = 1;

#ifndef PSPOT_TABLE_MODULE_

  // First prepare the client configuration data
  u_byte_t checksum[16];
  memset(checksum, 0, sizeof(checksum));
  if (Base::ComputeMD5Checksum("client_versions.conf",
                               checksum) == -1)
  {
    // Cannot open the configuration file!!!
    // Will send 0 as the checksum and the new file
    // will automatically downloaded.
    AfxMessageBox("The client configuration file 'client_versions.conf' was not "
                  "found. This file must be located in the PokerSpot client installation directory.");
    return 0;
  }

  PDULoungeLogin pdu(username, password,
                     0, // platform == WIN32
                     checksum);

  // Username & password are saved when 
  // receiving Accept PDU!

  rc = Network::SendPDU(socket_, pdu);

#endif

  return rc;
}


int LoungeServer::sendLoungeLogoutPDU()
{
  Global::SetCloseConnection(true);
  PDULoungeLogout pdu(Username_, Password_);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendLoungeSurveyPDU()
{
  PDULoungeSurvey pdu(Username_, Password_);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendLoungeSitDownPDU(USHORT tablenumber)
{
  PDULoungeSitDown pdu(Username_, Password_, tablenumber);
  return Network::SendPDU(socket_, pdu);
}

int LoungeServer::sendLoungeObservePDU(USHORT tablenumber)
{
  PDULoungeObserve pdu(tablenumber);
  return Network::SendPDU(socket_, pdu);
}

int LoungeServer::sendLoungeCancelSitDownPDU(USHORT tablenumber)
{
  PDULoungeCancelSitDown pdu(Username_, Password_, tablenumber);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendLoungeLogoutCertainPDU()
{
  PDULoungeLogoutCertain pdu(Username_, Password_);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendVerboseTableQueryPDU(USHORT tablenumber)
{
  PDUVerboseTableQuery pdu(Username_, Password_, tablenumber);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendTournamentDetailsQueryPDU(USHORT n)
{
  PDUTournamentDetailsQuery pdu(n);
  return Network::SendPDU(socket_, pdu);
}

int LoungeServer::sendHibernatePDU(bool b)
{
#if 0
  PDUHibernate pdu(b);
  return Network::SendPDU(socket_, pdu);
#else
  return 0;
#endif
}


int LoungeServer::sendLoungeTableLogoutPDU(USHORT tablenumber)
{
  Global::SetCloseConnection(true);
  PDULoungeTableLogout pdu(Username_,Password_, tablenumber);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendBalanceQueryPDU()
{
  PDUBalanceQuery pdu;
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendCardQueryPDU()
{
  PDUCreditCardQuery pdu;
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendCCardQueryPDU2(bool typesToo)
{
  PDUCreditCardQuery2 pdu(typesToo);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendCCardEditPDU(const std::list<CreditCardData>& d)
{
  PDUCreditCardEdit pdu(d);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendBuyInRequestPDU(u_int16_t card_index,
                                      const CChips& amount)
{
  PDULoungeBuyInRequest pdu(card_index, amount);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendCashOutRequestPDU(const CChips& amount)
{
  PDULoungeCashOutRequest pdu(amount);
  return Network::SendPDU(socket_, pdu);
}


int LoungeServer::sendTransactionQueryPDU()
{
  PDULoungeTransactionQuery pdu;
  return Network::SendPDU(socket_, pdu);
}

//
// Table Server Implementation
//


TableServer::TableServer(SOCKET s)
  :
  Server  (s),
  sequencenumber_(0)
{}


TableServer* TableServer::ConnectToTable(HWND  hWnd,
                                         UINT  host,
                                         short port)
{
  TableServer* pServer = 0;

#ifdef PSPOT_TABLE_MODULE_

  // MakeConnection attempts to unnecessarily resolve the host address
  // which can fail. connectsock works better.

  SOCKET s = connectsock(host, port);

  if (s == INVALID_SOCKET)
  {
    CString s, msg;
    s.Format(ErrFmt_, ErrCode_);
    msg.Format(g_szErrFmtUINT, host, port, s);
    if (CMainWnd::Instance())
      CMainWnd::Instance()->MessageBox(msg, NULL, MB_OK|MB_ICONEXCLAMATION);
    else
      AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
  }
  else
  {
    pServer = new TableServer(s);

    // Redirect incoming PDUs - 
    // NOTE: this is not a good solution!!!
    ThreadInfo_.hWnd = hWnd;
    ThreadInfo_.bQuit = FALSE;      
    ThreadInfo_.bRunning = FALSE;      
    ThreadInfo_.s = s;
    pServer->pSocketReader_ = StartSocketReaderThread(&ThreadInfo_);
  }

#endif

  return pServer;
}


TableServer* TableServer::ConnectToTable(HWND        hWnd,
                                         const char* host,
                                         short       port)
{
  TableServer* pServer = 0;

#ifdef PSPOT_TABLE_MODULE_

  SOCKET s = MakeConnection(host, htons(port));
  if (s == INVALID_SOCKET)
  {
    CString s, msg;
    s.Format(ErrFmt_, ErrCode_);
    msg.Format(g_szErrFmt, host, port, s);
    if (CMainWnd::Instance())
      CMainWnd::Instance()->MessageBox(msg, NULL, MB_OK|MB_ICONEXCLAMATION);
    else
      AfxMessageBox(msg, MB_OK|MB_ICONEXCLAMATION);
  }
  else
  {
    pServer = new TableServer(s);

    ThreadInfo_.hWnd = hWnd;
    ThreadInfo_.bQuit = FALSE;      
    ThreadInfo_.bRunning = FALSE;      
    ThreadInfo_.s = s;
    pServer->pSocketReader_ = StartSocketReaderThread(&ThreadInfo_);
  }

#endif

  return pServer;
}


int TableServer::sendSetupTableLogin(const char* username,
                                     const char* password)
{
  // Remember username & password
  Server::Username(username);
  Server::Password(password);

  PDUSetupTableLogin pdu(username, password);
  int rc = Network::SendPDU(socket_, pdu);

#ifdef PSPOT_TABLE_MODULE_
  if (rc != SOCKET_ERROR)
  {
    if (CTableView::Instance())
      CTableView::Instance()->recreateLeaveTableButton(ID_LOGOUT);
  }
#endif

  return rc;
}


int TableServer::sendSetupTableLogout()
{
  Global::SetCloseConnection(true);

  int rc = 0;

  if (Global::IsObserver())
  {
    rc = sendSetupObserverLogout();
  }
  else
  {
    PDUSetupTableLogout pdu(Username_, Password_);
    rc = Network::SendPDU(socket_, pdu);
  }

  // Start timer which will cause an automatic exit after
  // logout is complete
  if (AfxGetMainWnd())
      AfxGetMainWnd()->SetTimer(ID_TIMER_LOGOUTEXIT, 1000, NULL);

  return rc;
}


int TableServer::sendSetupObserverLogin(const char* username,
                                        const char* password)
{
  // Remember username & password
  Server::Username(username);
  Server::Password(password);

  PDUSetupObserverLogin pdu(username, password);
  int rc = Network::SendPDU(socket_, pdu);

#ifdef PSPOT_TABLE_MODULE_
  if (rc != SOCKET_ERROR)
  {
    if (CTableView::Instance())
      CTableView::Instance()->recreateLeaveTableButton(ID_LOGOUT);
  }
#endif

  return rc;
}


int TableServer::sendSetupObserverLogout()
{
  // Start timer which will cause an automatic exit after
  // logout is complete
  if (AfxGetMainWnd())
    AfxGetMainWnd()->SetTimer(1, 1000, NULL);

  PDUSetupObserverLogout pdu(Username_, Password_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendSetupBuyInRequestPDU(const CChips& chips)
{
  PDUSetupBuyInRequest pdu(Username_, Password_, chips);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendSetupHandLogRequestPDU(USHORT      hands,
                                            const char* emailaddr)
{
  PDUHandLogRequest pdu(hands, emailaddr, strlen(emailaddr));
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendSetupBuyinMore()
{
  PDUBuyInMore pdu;
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayActionCallPDU()
{
  PDUAction pdu(Username_, Password_, PDUAction::Call, sequencenumber_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayActionFoldPDU()
{
  PDUAction pdu(Username_, Password_, PDUAction::Fold, sequencenumber_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayActionRaisePDU()
{
  PDUAction pdu(Username_, Password_, PDUAction::Raise, sequencenumber_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayActionSpreadRaisePDU(const CChips& raise)
{
  PDUActionSpreadLimit pdu(Username_, Password_, PDUAction::Raise, raise, sequencenumber_);
  return Network::SendPDU(socket_, pdu);
}

int TableServer::sendPlaySitInPDU()
{
  PDUSitIn pdu(Username_, Password_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlaySitOutPDU()
{
  PDUSitOut pdu(Username_, Password_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayPostBlindPDU()
{
  PDUPostBlind pdu(Username_, Password_, sequencenumber_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayRefuseBlindPDU()
{
  PDURefuseBlind pdu(Username_, Password_, sequencenumber_);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayShowCardsPDU()
{
  PDUShowCards pdu(Username_, Password_, 1);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendPlayHideCardsPDU()
{
  PDUShowCards pdu(Username_, Password_, 0);
  return Network::SendPDU(socket_, pdu);
}


int TableServer::sendHeartBeatPDU()
{
  PDUHeartBeat pdu;
  return Network::SendPDU(socket_, pdu);
}


} // Network