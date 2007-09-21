#ifndef __network_server_h__
#define __network_server_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Network, class Server
  Created  : 4.11.1999

  OVERVIEW : Networking support for Texas Holdem. Class server is a kind
             of a proxy: it represents the PokerSpot server as if it was
             in-process.

 ****************************************************************************/

#include "network/network.h"
#include "network/pducreditcardedit.h"
#include <list>


class CChips;


BEGIN_NAMESPACE(Network)


class Server
{
public:
  // Log in to a server
  static Server* Connect(HWND hWnd, const char* address, short port);
  static void    SetWindow(HWND hWnd) { ThreadInfo_.hWnd = hWnd; }
  static void    Quit();
  virtual void   closeConnection();

  static CString GetLastError() { CString s; s.Format(ErrFmt_, ErrCode_); return s; }
  static int     GetLastErrCode() { return ErrCode_; }

  virtual ~Server();

  static CString Username() { return Username_; }
  static CString Password() { return Password_; }
  static void Username(const CString& un) { Username_ = un; }
  static void Password(const CString& pw) { Password_ = pw; }

private:  
  Server(SOCKET s);
  
protected:
  int sendPDU(PDUHeader*);

  SOCKET  socket_;
  static CString Username_;
  static CString Password_;

public:
  //
  // Common PDUs
  //
  virtual int sendChatPDU(const CString& msg);
  virtual int sendPongPDU(u_int32_t data);

public:
  static CString ErrFmt_;
  static int     ErrCode_;

public:
  static SOCKET MakeConnection(const char* address, short port);
  static SOCKET MakeConnection(UINT host, short port);

protected:

  friend class LoungeServer;
  friend class TableServer;
  // Socket reader thread
  CWinThread*                pSocketReader_;
  static Network::ThreadInfo ThreadInfo_;
};


// This server is used for lounge interaction
class LoungeServer : public Server
{
  LoungeServer(SOCKET);

public:
  static LoungeServer* ConnectToLounge(HWND hWnd,
                                       const char* address,
                                       short port);
  static LoungeServer* ConnectToLounge(HWND hWnd,
                                       ULONG ipAddress,
                                       short port);

  //
  // Lounge PDUs
  //
  int sendLoungeHandshake(const char* username);
  int sendLoungeLoginPDU(const char* username,
                         const char* password);
  int sendLoungeLogoutPDU();
  int sendLoungeSurveyPDU();
  int sendLoungeSitDownPDU(USHORT tablenumber);
  int sendLoungeObservePDU(USHORT tablenumber);
  int sendLoungeCancelSitDownPDU(USHORT tablenumber);
  int sendLoungeLogoutCertainPDU();
  int sendLoungeTableLogoutPDU(USHORT tablenumber);

  int sendBalanceQueryPDU();
  int sendCardQueryPDU();
  int sendBuyInRequestPDU(u_int16_t card_index,
                          const CChips& amount);
  int sendCashOutRequestPDU(const CChips& amount);
  int sendTransactionQueryPDU();

  int sendVerboseTableQueryPDU(USHORT tablenumber);
  int sendTournamentDetailsQueryPDU(USHORT tournament);

  int sendHibernatePDU(bool);

  int sendCCardQueryPDU2(bool typesToo);
  int sendCCardEditPDU(const std::list<CreditCardData>&);
};


// This server is used for game play interaction
class TableServer : public Server
{
  TableServer(SOCKET);

public:
  static TableServer* ConnectToTable(HWND        hWnd,
                                     UINT        host,
                                     short       port);
  static TableServer* ConnectToTable(HWND        hWnd,
                                     const char* host,
                                     short       port);

  // Set current sequence number
  void setSequenceNumber(u_int16_t n) { sequencenumber_ = n; }
  u_int16_t getSequenceNumber() const { return sequencenumber_; }

  //
  // Setup interaction
  //
  int sendSetupTableLogin(const char* username,
                          const char* password);
  int sendSetupTableLogout();
  int sendSetupBuyInRequestPDU(const CChips& chips);

  int sendSetupHandLogRequestPDU(USHORT      hands,
                                 const char* emailaddr);
  int sendSetupBuyinMore();

  int sendSetupObserverLogin(const char* username,
                             const char* password);
  int sendSetupObserverLogout();

  //
  // Game play interaction
  //
  int sendPlaySitInPDU();
  int sendPlaySitOutPDU();
  int sendPlayPostBlindPDU();
  int sendPlayRefuseBlindPDU();

  int sendPlayShowCardsPDU();
  int sendPlayHideCardsPDU();

  int sendPlayActionCallPDU();
  int sendPlayActionFoldPDU();
  int sendPlayActionRaisePDU();
  int sendPlayActionSpreadRaisePDU(const CChips& raise);

  int sendHeartBeatPDU();

private:
  u_int16_t sequencenumber_;
};


END_NAMESPACE(Network)

#endif