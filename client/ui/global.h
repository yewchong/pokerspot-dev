#ifndef __psi_global_h__
#define __psi_global_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Global
  Created  : 01/19/2000

  OVERVIEW : Wrapper class for global application data.

 ****************************************************************************/

#include "network/pdutournamentparams.h"

namespace Network
{
  class LoungeServer;
  class TableServer;
}

enum GameType
{
  GT_Holdem    = 0,
  GT_SevenStud = 1,
  GT_Omaha     = 2 
};


class Global
{
public:
  Global();
  virtual ~Global();

  BOOL initInstance();
  int  exitInstance();

  static GameType GetGameType();
  static void     SetGameType(GameType);
  static const char* GetGameName();

  static Global& Instance();
  static LPCTSTR AppName ();
  static LPCTSTR GetFontFace();
  static CFont*  GetFont();
  static CFont*  GetBoldFont();
  static int     GetCharHeight();

  static UINT    GetAnimationTime();
  static void    SetAnimationTime(UINT);

  static void    SetCloseConnection(bool);
  static bool    CloseConnection();

  CFont*         getFont() const;
  CFont*         getBoldFont() const;

  static Network::LoungeServer* LoungeServer();
  static Network::TableServer* TableServer();

  bool loginToLoungeServer(HWND           hWnd,
                           const CString& host,
                           int            port,
                           const CString& username,
                           const CString& passwd);
  bool loginToLoungeServer(HWND           hWnd,
                           ULONG          hostIpAddress,
                           int            port,
                           const CString& username,
                           const CString& passwd);
  bool loginToLoadBalancer(HWND           hWnd,
                           const CString& host,
                           int            port,
                           const CString& username);
  bool loginToPlayMoneyServer(HWND           hWnd,
                              const CString& host,
                              int            port,
                              const CString& username,
                              const CString& passwd);
  bool loginToRealMoneyServer(HWND           hWnd,
                              const CString& host,
                              int            port,
                              const CString& username,
                              const CString& passwd);
  void closeLoungeServer();
  bool loginToTableServer(ULONG          host,
                          USHORT         port,
                          const CString& username,
                          const CString& passwd);
  bool loginToTableServer(const CString& host,
                          int            port,
                          const CString& username,
                          const CString& passwd);
  void closeTableServer();
  void closeConnections();

  static bool IsObserver();
  static void SetObserver(bool);

  static bool IsTournament();
  static void SetIsTournament(bool);

  static Network::BetStructure GetBetting();
  static void SetBetting(Network::BetStructure);

  void addActionTimeout();
  void resetActionTimeouts();
  int  getActionTimeouts() const;

  bool isChatEnabled() const;
  CString chatEnabledMsg() const;
  void enableChat(const CString& msg);

private:
  CFont font_;
  CFont fontBold_;  
  static int iCharHeight_;
  UINT animationTime_;

  static bool CloseConnection_;

  // Client's proxy to the PokerSpot server(s)
  Network::LoungeServer* pLoungeServer_;
  Network::LoungeServer* pPlayMoneyServer_;
  Network::LoungeServer* pRealMoneyServer_;
  Network::TableServer* pTableServer_;

  bool isObserver_;
  bool isTournament_;

  GameType gameType_;
  Network::BetStructure betting_;

  int actionTimeouts_;

  CString chatEnabled_;
};

inline
Network::LoungeServer* Global::LoungeServer()
{
  return Instance().pLoungeServer_;
}


inline
Network::TableServer* Global::TableServer()
{
  return Instance().pTableServer_;
}

extern Global g_Global;

inline
Global& Global::Instance()
{
  return g_Global;
}

inline UINT Global::GetAnimationTime()
{
    return Instance().animationTime_;
}

inline void Global::SetAnimationTime(UINT animTime)
{
    Instance().animationTime_ = animTime;
}

inline bool Global::IsObserver()
{
    return Instance().isObserver_;
}

inline void Global::SetObserver(bool b)
{
    Instance().isObserver_ = b;
}


#endif 
