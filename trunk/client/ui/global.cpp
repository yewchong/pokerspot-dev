/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Global
  Created  : 01/19/2000

  OVERVIEW : Wrapper class for global application data.

 ****************************************************************************/

#include "stdafx.h"
#include "global.h"
#include "base/gamelogic.h"
#include "base/misc.h"
#include "ui/tableview.h"
#include "ui/actionbar.h"
#include "ui/logindlg.h"
#include "network/server.h"
#include <mmsystem.h>


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
  // Module globals

  LPCTSTR g_szAppName  = _T("PokerSpot Table");
  LPCTSTR g_szFontFace = _T("Arial");
  LPCTSTR g_szNotEnoughMemory = _T("Not enough memory to run!");

  const int g_FontSize = 9;
}

namespace
{
  void SanityCheckMax(DWORD& d, const DWORD dwMax)
  {
    if (d > dwMax)
      d = dwMax;
  }
}

Global g_Global;
int Global::iCharHeight_ = 0;
bool Global::CloseConnection_ = false;


Global::Global()
  :
  animationTime_(400),
  pLoungeServer_(0),
  pPlayMoneyServer_(0),
  pRealMoneyServer_(0),
  pTableServer_ (0),
  isObserver_   (false),
  isTournament_ (false),
  gameType_     (GT_Holdem),
  betting_      (Network::BS_Limit),
  actionTimeouts_(0)
{}


Global::~Global()
{}


BOOL Global::initInstance()
{
  BOOL rc = TRUE;
  return rc;
}


int Global::exitInstance()
{
  closeConnections();

#ifdef _DEBUG
  // Detect PDU memory leaks
  TRACE1("PDU Allocation Balance: %d\n", Network::PDUHeader::AllocBalance_);
#endif

  return 0;
}


GameType Global::GetGameType()
{
  return Instance().gameType_;
}

void Global::SetGameType(GameType gt)
{
#ifdef PSPOT_TABLE_MODULE_
  bool reload = Instance().gameType_ != gt;
  Instance().gameType_ = gt;

  if (reload)
  {
    CTableView* pView = CTableView::Instance();
    if (pView)
    {
      pView->reloadConfiguration();
      pView->resetCaption();
      CActionBar::ResetAnteCheckbox();
    }
  }
#endif
}

const char* Global::GetGameName()
{
  switch (Instance().gameType_)
  {
  case GT_SevenStud:
    return "7-Card Stud";

  case GT_Omaha:
    return "Omaha";

  default:
  case GT_Holdem:
    return "Texas Hold'em";
  }
}


LPCTSTR Global::AppName()
{
  return g_szAppName;
}


LPCTSTR Global::GetFontFace()
{
  return g_szFontFace;
}


CFont* Global::GetFont()
{
  return Instance().getFont();
}


CFont* Global::GetBoldFont()
{
  return Instance().getBoldFont();
}


CFont* Global::getFont() const
{
  Global* ncThis = const_cast<Global*>(this);

  if (!font_.GetSafeHandle())
  {
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    ncThis->font_.CreateFont(-((g_FontSize * GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY)) / 72),
                             0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0,
                             Global::GetFontFace());
  }

  return &(ncThis->font_);
}


CFont* Global::getBoldFont() const
{
  Global* ncThis = const_cast<Global*>(this);

  if (!fontBold_.GetSafeHandle())
  {
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    ncThis->fontBold_.CreateFont(-((g_FontSize * GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY)) / 72),
                                 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0,
                                 Global::GetFontFace());
  }

  return &(ncThis->fontBold_);
}

int Global::GetCharHeight()
{
  if (iCharHeight_ == 0)
  { // First call computes the char height.
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CFont* pOldFont = dc.SelectObject(Global::GetFont());
    TEXTMETRIC tm;
    ::ZeroMemory(&tm, sizeof(tm));
    dc.GetTextMetrics(&tm);
    dc.SelectObject(pOldFont);
    iCharHeight_ = tm.tmHeight;
  }
  return iCharHeight_;
}

bool Global::loginToLoungeServer(HWND           hWnd,
                                 const CString& host,
                                 int            port,
                                 const CString& username,
                                 const CString& passwd)
{
#ifndef PSPOT_TABLE_MODULE_
  SetCloseConnection(false);
  CWaitCursor wait;

  if (!pLoungeServer_)
    pLoungeServer_ = Network::LoungeServer::ConnectToLounge(hWnd, (LPCTSTR)host, port);

  if (pLoungeServer_)
    pLoungeServer_->sendLoungeLoginPDU(username, passwd);

  return pLoungeServer_ != NULL;
#else
  return FALSE;
#endif
}


bool Global::loginToLoungeServer(HWND           hWnd,
                                 ULONG          hostIpAddress,
                                 int            port,
                                 const CString& username,
                                 const CString& passwd)
{
#ifndef PSPOT_TABLE_MODULE_
  SetCloseConnection(false);
  CWaitCursor wait;

  if (!pLoungeServer_)
    pLoungeServer_ = Network::LoungeServer::ConnectToLounge(hWnd,
                                                            hostIpAddress, 
                                                            port);

  // Save address & port in case of login retries
  CLoginDlg::IpAddress_ = hostIpAddress;
  CLoginDlg::Port_ = port;

  if (pLoungeServer_)
    pLoungeServer_->sendLoungeLoginPDU(username, passwd);

  return pLoungeServer_ != NULL;
#else
  return FALSE;
#endif
}

bool Global::loginToLoadBalancer(HWND           hWnd,
                                 const CString& host,
                                 int            port,
                                 const CString& username)
{
#ifndef PSPOT_TABLE_MODULE_
  SetCloseConnection(false);
  CWaitCursor wait;

  if (!pLoungeServer_)
    pLoungeServer_ = Network::LoungeServer::ConnectToLounge(hWnd, (LPCTSTR)host, port);

  if (pLoungeServer_)
    pLoungeServer_->sendLoungeHandshake(username);

  return pLoungeServer_ != NULL;
#else
  return FALSE;
#endif
}


bool Global::loginToPlayMoneyServer(HWND hWnd,
                                    const CString& host,
                                    int            port,
                                    const CString& username,
                                    const CString& passwd)
{
#ifndef PSPOT_TABLE_MODULE_
  SetCloseConnection(false);
  CWaitCursor wait;

  if (!pPlayMoneyServer_)
    pPlayMoneyServer_ = Network::LoungeServer::ConnectToLounge(hWnd, (LPCTSTR)host, port);

  if (pPlayMoneyServer_)
    pPlayMoneyServer_->sendLoungeLoginPDU(username, passwd);

  return pPlayMoneyServer_ != NULL;
#else
  return FALSE;
#endif
}

bool Global::loginToRealMoneyServer(HWND           hWnd,
                                    const CString& host,
                                    int            port,
                                    const CString& username,
                                    const CString& passwd)
{
#ifndef PSPOT_TABLE_MODULE_
  SetCloseConnection(false);
  CWaitCursor wait;

  if (!pRealMoneyServer_)
    pRealMoneyServer_ = Network::LoungeServer::ConnectToLounge(hWnd, (LPCTSTR)host, port);

  if (pRealMoneyServer_)
    pRealMoneyServer_->sendLoungeLoginPDU(username, passwd);

  pLoungeServer_ = pRealMoneyServer_;

  return pRealMoneyServer_ != NULL;
#else
  return FALSE;
#endif
}

bool Global::loginToTableServer(ULONG          host,
                                USHORT         port,
                                const CString& username,
                                const CString& passwd)
{
  SetCloseConnection(false);
  CWaitCursor wait;

  CView* pView = Base::GetActiveView();

  if (!pTableServer_)
    pTableServer_ = Network::TableServer::ConnectToTable(pView ? pView->GetSafeHwnd() : NULL,
                                                         host, port);

  if (pTableServer_)
  {
    if (isObserver_)
      pTableServer_->sendSetupObserverLogin(username, passwd);
    else
      pTableServer_->sendSetupTableLogin(username, passwd);
  }

  return pTableServer_ != NULL;
}


bool Global::loginToTableServer(const CString& host,
                                int            port,
                                const CString& username,
                                const CString& passwd)
{
  SetCloseConnection(false);
  CWaitCursor wait;

  CView* pView = Base::GetActiveView();

  if (!pTableServer_)
    pTableServer_ = Network::TableServer::ConnectToTable(pView ? pView->GetSafeHwnd() : NULL,
                                                         host, port);

  if (pTableServer_)
  {
    if (isObserver_)
      pTableServer_->sendSetupObserverLogin(username, passwd);
    else
      pTableServer_->sendSetupTableLogin(username, passwd);
    // repaint window now
    if (pView)
      pView->Invalidate();
  }

  return pTableServer_ != NULL;
}


void Global::closeTableServer()
{
  Base::GameLogic::Instance().setNextState(0);
  Network::Server::Quit();

  if (pTableServer_)
    pTableServer_->closeConnection();
  delete pTableServer_;
  pTableServer_ = 0;
}


void Global::closeLoungeServer()
{
  Base::GameLogic::Instance().setNextState(0);
  Network::Server::Quit();

  if (pLoungeServer_)
    pLoungeServer_->closeConnection();
  delete pLoungeServer_;
  pLoungeServer_ = 0;
}


void Global::closeConnections()
{
  Network::Server::Quit();
  closeLoungeServer();
  closeTableServer();
}


bool Global::IsTournament()
{
    return Instance().isTournament_;
}

void Global::SetIsTournament(bool b)
{
    Instance().isTournament_ = b;
}


void Global::SetCloseConnection(bool b)
{
    CloseConnection_ = b;
}

bool Global::CloseConnection()
{
    return CloseConnection_;
}


Network::BetStructure Global::GetBetting()
{
    return Instance().betting_;
}

void Global::SetBetting(Network::BetStructure bs)
{
    Instance().betting_ = bs;
}


void Global::addActionTimeout()
{
  actionTimeouts_++;

#ifdef _DEBUG
  CString s;
  s.Format("Global::addActionTimeout: %i\n", actionTimeouts_);
  OutputDebugString(s);
#endif

}


void Global::resetActionTimeouts()
{
#ifdef _DEBUG
  OutputDebugString("Reset action timeouts\n");
#endif
  actionTimeouts_ = 0;
}


int Global::getActionTimeouts() const
{
  return actionTimeouts_;
}



bool Global::isChatEnabled() const
{
  // If enable string is empty, chat is enabled
  return (chatEnabled_.GetLength() == 0);
}


void Global::enableChat(const CString& msg)
{
  chatEnabled_ = msg;
}


CString Global::chatEnabledMsg() const
{
  return chatEnabled_;
}