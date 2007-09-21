/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMainWnd
  Created  : 25.10.1999

  OVERVIEW : CMainWnd is the main window class derived
             from CFrameWnd.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/mainwnd.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/logindlg.h"
#include "ui/motddlg.h"
#include "ui/optionsdlg.h"
#include "ui/aboutdlg.h"
#include "ui/promptinputdlg.h"
#include "ui/global.h"
#include "ui/registry.h"
#include "ui/soundmanager.h"
#include "ui/cmdid.h"
#include "ui/splashwnd.h"
#include "ui/maildoc.h"
#include "ui/inputdlg.h"
#include "base/gamelogic.h"
#include "base/gamestate.h"
#include "base/misc.h"
#include "network/network.h"
#include "network/server.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
  //
  // Module globals
  //

  static UINT g_Indicators[] =
  {
    ID_SEPARATOR,        // status line indicator
    ID_INDICATOR_ONLINE, // "Connected" indicator
  };

  LPCTSTR g_szConfirmQuit = _T("Really quit?");
  LPCTSTR g_szConfirmLogout = _T("Leave the table now?");

 
  //
  // Module helper functions
  //

CString PromptString(CWnd* pWnd, const char* prompt)
{
  CString s;
  CPromptInputDlg dlg(pWnd);
  dlg.prompt_ = prompt;
  if (dlg.DoModal())
    s = dlg.input_;
  return s;
}


CString GetHandLogEmailAddress(CWnd* pWnd)
{
  CString s = Registry::GetHandLogEmailAddress("");
  if (s.IsEmpty())
  {
    s = PromptString(pWnd, "Please enter the email address to which "
                           "the hand log will be sent.");
    if (!s.IsEmpty())
      Registry::SetHandLogEmailAddress(s);
  }
  return s;
}

} // namespace


CMainWnd* CMainWnd::pInstance_ = 0;


CMainWnd::CMainWnd(UINT floorMapMessage)
  :
  floorMapMessage_(floorMapMessage)
{
  pInstance_ = this;
}


CMainWnd::~CMainWnd()
{
  pInstance_ = 0;
}


BEGIN_MESSAGE_MAP(CMainWnd, CFrameWnd)
    //{{AFX_MSG_MAP(CMainWnd)
    ON_WM_CREATE()
    ON_WM_ERASEBKGND()
    ON_WM_SIZE()
    ON_WM_CLOSE()
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_LOGIN, OnLogin)
    ON_UPDATE_COMMAND_UI(ID_LOGIN, OnUpdateLogin)
    ON_COMMAND(ID_LOGOUT, OnLogout)
    ON_UPDATE_COMMAND_UI(ID_LOGOUT, OnUpdateLogout)
    ON_COMMAND(ID_MOTD, OnMotd)
    ON_COMMAND(ID_TABLE, OnTable)
    ON_WM_GETMINMAXINFO()
    ON_COMMAND(ID_SOUNDTOGGLE, OnSoundtoggle)
    ON_UPDATE_COMMAND_UI(ID_SOUNDTOGGLE, OnUpdateSoundtoggle)
    ON_COMMAND(ID_TESTDOWNLOAD, onTestDownload)
    ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_REQUEST5HANDLOG, OnRequest5handlog)
	ON_UPDATE_COMMAND_UI(ID_REQUEST5HANDLOG, OnUpdateRequest5handlog)
	ON_COMMAND(ID_REQUESTPREVHAND, OnRequestprevhand)
	ON_UPDATE_COMMAND_UI(ID_REQUESTPREVHAND, OnUpdateRequestprevhand)
	ON_WM_MOVE()
	ON_COMMAND(ID_BUYIN, OnBuyin)
	ON_UPDATE_COMMAND_UI(ID_BUYIN, OnUpdateBuyin)
	ON_WM_TIMER()
	ON_COMMAND(ID_TESTLOGINOUT, OnTestloginout)
    ON_WM_MEASUREITEM()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
    ON_UPDATE_COMMAND_UI(ID_INDICATOR_ONLINE, OnUpdateOnlineIndicator)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers

BOOL CMainWnd::Create(BOOL show_splash, LPCTSTR lpszClassName, LPCTSTR lpszWindowName)
{
  BOOL rc = CFrameWnd::Create(lpszClassName, Global::AppName());

  if (rc)
  {
    restoreSettings();
    actionBar_.ShowWindow(SW_SHOWNORMAL);
    chatDlg_.ShowWindow(SW_SHOWNORMAL);

    if (show_splash)
    {
/*
      if (!CSplashWnd::ShowSplashScreen("splash_holdem.jpg",
                                        TRUE, // post login when close
                                        TRUE, // close on timer
                                        this))
      {
*/
        if (AfxGetMainWnd())
          AfxGetMainWnd()->PostMessage(WM_COMMAND,
                                       MAKEWPARAM(ID_LOGIN, 0),
                                       0);
    }
  }

  return rc;
}


//
// Override DestroyWindow to save the settings
// and quit.
//
BOOL CMainWnd::DestroyWindow() 
{
  saveSettings();
  return CFrameWnd::DestroyWindow();
}


//
// Override OnCreateClient to create and initialize
// the splitter window and the panes.
//
BOOL CMainWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  // Start with table view - limits will be replaced
  // when the command line is interpreted
  CView* pView = new CTableView(Global::GetGameName(), DEFAULT_LOW, DEFAULT_HI);
  if (pView)
  {
    if (pView->Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
                      CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, pContext))
    {
      pView->OnInitialUpdate();
      SetActiveView(pView);
    }
  }

  BOOL rc = CFrameWnd::OnCreateClient(lpcs, pContext);

  ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_FRAMECHANGED);

  return rc;
}


//
// Override OnCreate to create the ActionBar and chat windows.
int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  if (!actionBar_.Create(this))
  {
    TRACE0("Failed to create action bar\n");
    return -1;
  }

  if (!chatDlg_.Create(NULL, NULL, WS_CHILD|WS_VISIBLE, rectDefault, this, IDD_CHAT))
  {
    TRACE0("Failed to create chat window\n");
    return -1;
  }

#define STATUSBAR_
#ifdef STATUSBAR_
  if (!statusBar_.Create(this,
                         WS_CHILD|WS_VISIBLE|CBRS_BOTTOM,
                         IDC_STATUSBAR) ||
      !statusBar_.SetIndicators(g_Indicators,
                                sizeof(g_Indicators)/sizeof(UINT)))
  {
    TRACE0("Failed to create status bar\n");
    return -1;      // fail to create
  }
  CString s;
  s.LoadString(ID_INDICATOR_OFFLINE);
  statusBar_.SetPaneText(1, s, TRUE);
  statusBar_.SetPaneInfo(1, ID_INDICATOR_ONLINE, 0, 45);
#endif

  // The action bar will be shown in restoreSettings()

  // Remove the maximize window capability
#ifdef NO_MAXIMIZE_STYLE_
  DWORD style = ::GetWindowLong(m_hWnd, GWL_STYLE);
  style &= ~WS_MAXIMIZEBOX;
  ::SetWindowLong(m_hWnd, GWL_STYLE, style);
#endif

  // Send heartbeat every 80 seconds
  if (SetTimer(ID_TIMER_HEARTBEAT, 80 * 1000, NULL) == 0)
  {
      AfxMessageBox("Unable to create timer - please quit some applications to free memory.");
  }

  return 0;
}


//
// Override OnEraseBkgn to do nothing
// to eliminate flicker.
// 
BOOL CMainWnd::OnEraseBkgnd(CDC* pDC) 
{
  return TRUE;
}


// 
// Override OnSize to adjust the view
//
void CMainWnd::OnSize(UINT nType, int cx, int cy) 
{
  CFrameWnd::OnSize(nType, cx, cy);

  CView* view = GetActiveView();
  if (view)
  {
    CRect rect, rectSB;

    GetClientRect(&rect);
    if (statusBar_.GetSafeHwnd())
    {
      statusBar_.GetWindowRect(&rectSB);
      ScreenToClient(&rectSB);
      rect.bottom = rectSB.top;
      view->MoveWindow(rect);
    }
  } 

  if (actionBar_.GetSafeHwnd() && actionBar_.isDocked())
  {
      actionBar_.attach();
  }

  if (chatDlg_.GetSafeHwnd() && chatDlg_.isDocked())
  {
      chatDlg_.attach();
  }
}


void CMainWnd::OnClose() 
{
  // If we have live connection, confirm, otherwise
  // quit without asking
  if (!Global::TableServer() ||
      AfxMessageBox(g_szConfirmQuit, MB_ICONQUESTION|MB_YESNO) == IDYES)
  {
    CMenu* pMenu = GetMenu();
    delete pMenu;
    SetMenu(0);
    CFrameWnd::OnClose();

    if (Global::TableServer())
    {
      if (Global::IsObserver())
        Global::TableServer()->sendSetupObserverLogout();
      else
        Global::TableServer()->sendSetupTableLogout();
    }
    Network::Server::Quit();
  } 
}


void CMainWnd::OnAppAbout() 
{
  CAboutDlg dlg;
  dlg.DoModal();
/*
  CSplashWnd::EnableSplashScreen(TRUE);
  CSplashWnd::ShowSplashScreen("splash_holdem.jpg",
                               FALSE, // post login?
                               FALSE, // quit on timer?
                               this);
*/
}


void CMainWnd::OnOptions() 
{
  COptionsDlg dlg(this);
  dlg.DoModal();
}


void CMainWnd::OnLogin() 
{
  CLoginDlg dlg(this);
  dlg.DoModal();
}


void CMainWnd::OnUpdateLogin(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(Global::TableServer() == NULL);
}


void CMainWnd::OnLogout() 
{
  if (AfxMessageBox(g_szConfirmLogout, MB_ICONQUESTION|MB_YESNO) == IDYES)
  {
    if (Global::TableServer())
    {
      if (Global::IsObserver())
        Global::TableServer()->sendSetupObserverLogout();
      else
        Global::TableServer()->sendSetupTableLogout();
    }
  }
}


void CMainWnd::OnUpdateLogout(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(Global::TableServer() != NULL);
}


void CMainWnd::OnMotd() 
{ 
#ifdef TEST_MOTD_
  EnableWindow(FALSE);
  CMotdDlg dlg(g_Motd, this);
  dlg.DoModal();
  EnableWindow(TRUE);	
#endif
}


/////////////////////////////////////////////////////////////////////////////
// CMainWnd application mfunctions

//
// Save current window state to registry.
//
void CMainWnd::saveSettings()
{

  if (IsIconic())
    return; // Don't save minized state!

  CRect r(0, 0, CW_USEDEFAULT, CW_USEDEFAULT);
  
  if (Base::IsMaximized(this))
    r.right = r.bottom = MW_Maximized;
  else
    GetWindowRect(&r);
  Registry::SetWndRect(r);

  if (::IsWindow(actionBar_.GetSafeHwnd()))
  {
    actionBar_.GetWindowRect(&r);
    Registry::SetActionBarPos(r.TopLeft());

    if (actionBar_.isDocked())
      Registry::SetActionBarDock(1);
    else
      Registry::SetActionBarDock(0);
  }

  if (::IsWindow(chatDlg_.GetSafeHwnd()))
  {
    chatDlg_.GetWindowRect(&r);
    Registry::SetChatRect(r);
    Registry::SetChatWndDock(chatDlg_.isDocked());
  }
}


//
// Restore window state from registry.
//
void CMainWnd::restoreSettings()
{
  int v = Registry::Get1_02(0);
  if (v == 0)
  {   // This is first restore since 1.02
      // Create nice default layout
      Registry::SetActionBarLayout(1);
      Registry::SetActionBarDock(1);
      Registry::SetChatWndDock(1); // dock right
      recreateActionBar();
  }
  Registry::Set1_02(1);

  const int DefaultWidth = 653;
  const int DefaultHeight = 518;
  CRect r(15, 15, 668, 591); 
  r = Registry::GetWndRect(r);

  CRect rectBar;
  actionBar_.GetClientRect(&rectBar);
  const int Margin = 5;

  if (r.right == MW_Maximized)
    ShowWindow(SW_MAXIMIZE);
  else if (r.right != -1 && r.bottom != -1)
  {
    MoveWindow(&r);
    ShowWindow(SW_SHOWNORMAL);
  }
  else
  {
    CRect rectWnd;
    GetWindowRect(&rectWnd);
    if (rectWnd.left < (rectBar.Width() + Margin))
    { 
      rectWnd.right = rectWnd.left + DefaultWidth;
      rectWnd.bottom = rectWnd.top + DefaultHeight;
      // Make sure action bar has enough room
      rectWnd.OffsetRect((rectBar.Width() + Margin) - rectWnd.left, 0);
      MoveWindow(&rectWnd);
    }
    ShowWindow(SW_SHOWNORMAL);
  }

  if (r.right == -1 || r.bottom == -1)
  { // by default position the actionbar to the left
    // of the main wnd
    GetWindowRect(&r);
    r.left -= (rectBar.Width() + Margin);
  }

  CPoint pos(r.left, r.top);
  pos = Registry::GetActionBarPos(pos);
  actionBar_.SetWindowPos(NULL, pos.x, pos.y, 0, 0,
                          SWP_NOSIZE|SWP_NOZORDER);

  if (Registry::GetActionBarDock(DefaultActionBarDock))
  {
      Registry::SetActionBarDock(DefaultActionBarDock);
      // Action bar must be made horizontal in
      // order to dock
      int layout = Registry::GetActionBarLayout(0);
      Registry::SetActionBarLayout(1);

      if (layout == 0)
      {
          recreateActionBar();
          Registry::SetActionBarLayout(0);
      }

      actionBar_.dock(TRUE);
      actionBar_.attach();
  }

  int dock = Registry::GetChatWndDock(DefaultChatWndDock);
  if (dock != 0)
  { 
      if (dock != 1 && dock != 2)
          dock = 1;
      Registry::SetChatWndDock(dock);
      chatDlg_.dock((CChatDlg::ChatDlg_DockMode)dock);
      if (v == 0)
          chatDlg_.attach();    
  }

  // position chat window right of main window
  if (v == 0)
  {
      GetWindowRect(&rectBar);
      if (rectBar.left > 100)
      {
          rectBar.OffsetRect(100 - rectBar.left, 0);
          MoveWindow(&rectBar);
      }
  }

  GetWindowRect(&rectBar);
  rectBar.top += GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYMENU);
  rectBar.bottom = rectBar.top +
                   int((3.0f / 4.0f * float(rectBar.Height())));
  rectBar.left = rectBar.right;
  rectBar.right = rectBar.left + 180;
  if (v == 0)
      r = rectBar;
  else
      r = Registry::GetChatRect(rectBar);
  if (r.left > GetSystemMetrics(SM_CXSCREEN) - 50)
  { // make sure it won't disappear out of the screen
    r.left = GetSystemMetrics(SM_CXSCREEN) - 50;
  }
  chatDlg_.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(),
                        SWP_NOZORDER);

  if (chatDlg_.isDocked())
      chatDlg_.attach();
}


void CMainWnd::recreateActionBar()
{
  CRect r(0, 0, 0, 0);
  if (actionBar_.GetSafeHwnd())
  {
    actionBar_.GetWindowRect(&r);
    actionBar_.DestroyWindow();
  }

  if (!actionBar_.Create(this))
  {
    TRACE0("Failed to create action bar\n");
    return;
  }

  if (!r.IsRectEmpty())
  {
    CRect r2;
    actionBar_.GetWindowRect(&r2);
    actionBar_.MoveWindow(r.left, r.top,
                          r2.Width(), r2.Height(),
                          FALSE);
    actionBar_.saveWidth();
  }

  actionBar_.resetCheckStates();

  actionBar_.ShowWindow(SW_NORMAL);
}

void CMainWnd::OnTable() 
{
  CView* pView = GetActiveView();
  if(!pView) return;
  if (pView && pView->IsKindOf(RUNTIME_CLASS(CTableView)))
    return;

  pView->DestroyWindow();

  pView = new CTableView(Global::GetGameName(), DEFAULT_LOW, DEFAULT_HI);
  if (pView)
  {
    CRect r;
    GetClientRect(&r);
    if (pView->Create(NULL, NULL, WS_VISIBLE|WS_CHILD,
                      r, this, AFX_IDW_PANE_FIRST))
    {
      pView->OnInitialUpdate();
      SetActiveView(pView);
      pView->Invalidate();

      // When the active view changes, the server must be notified
      Network::Server::SetWindow(pView->GetSafeHwnd());
    }
  }
}


void CMainWnd::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
  CView* pView = GetActiveView();
  if (pView && pView->IsKindOf(RUNTIME_CLASS(CTableView)))
  {
    CTableView* pTable = static_cast<CTableView*>(pView);
    pTable->getMinMaxInfo(lpMMI);
  }

  // If the action bar is docked, don't allow resizing
  // smaller than action bar
  if (actionBar_.GetSafeHwnd() && actionBar_.isDocked())
  {
      if (actionBar_.getMinWidth() + 2 > lpMMI->ptMinTrackSize.x)
        lpMMI->ptMinTrackSize.x = actionBar_.getMinWidth() + 2;
  }

  CFrameWnd::OnGetMinMaxInfo(lpMMI);
}


void CMainWnd::OnSoundtoggle() 
{
  SndMan::Instance()->enable(!SndMan::Instance()->isEnabled());
}


void CMainWnd::OnUpdateSoundtoggle(CCmdUI* pCmdUI) 
{
  if (pCmdUI)
    pCmdUI->SetCheck(SndMan::Instance()->isEnabled() ? 1 : 0);	
}


void CMainWnd::OnUpdateOnlineIndicator(CCmdUI* pCmdUI) 
{
  CString s;
  BOOL isOnline = FALSE;
  if (Global::TableServer())
  {
    s.LoadString(ID_INDICATOR_ONLINE);
    isOnline = TRUE;
  }
  else
  {
    s.LoadString(ID_INDICATOR_OFFLINE);
  }
  pCmdUI->SetText(s);
  pCmdUI->Enable(TRUE);
  if (CTableView::Instance())
    CTableView::Instance()->setOnlineStatus(isOnline);
}


// Override to see whether the floor map client
// sent us a message.
BOOL CMainWnd::PreTranslateMessage(MSG* pMsg) 
{
  if (floorMapMessage_ != 0 &&
      pMsg->message == floorMapMessage_)
  {
    char buf[100];
    ATOM a = pMsg->lParam;
    ::GlobalGetAtomName(a, buf, 99);
    ::GlobalDeleteAtom(a);
    floorMapMessage_ = 0;
    CString s;
    s.Format("Got the message: %s\n", buf);
    //AfxMessageBox(s);
    return TRUE;
  }
  else  
    return CFrameWnd::PreTranslateMessage(pMsg);
}


void CMainWnd::SetStatusText(const CString& s)
{
  CMainWnd* pWnd = Instance();
  if (pWnd)
  {
    CWnd* pStatusBar = pWnd->GetControlBar(IDC_STATUSBAR);
    if (pStatusBar)
      pStatusBar->SetWindowText(s);
  }
}


void CMainWnd::onTestDownload()
{
}

BOOL CMainWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
  BOOL rc = FALSE;

  UINT cmd = LOWORD(wParam);
  if (cmd == ID_TOGGLEACTIONBARLAYOUT ||
      cmd == ID_TOGGLEACTIONBARDOCK)
  {
    toggleActionBar(cmd);
    rc = TRUE;
  }
  else if (cmd == ID_TOGGLECHATWNDDOCK)
  {
    toggleChatWnd(HIWORD(wParam));
    rc = TRUE;
  }

  if (!rc)
    rc = CFrameWnd::OnCommand(wParam, lParam);

  return rc;
}

void CMainWnd::toggleActionBar(UINT cmd)
{
    // Don't do the layout toggle if the player is
    // being queried for an action - it would disable
    // the controls and leave the player without chance
    // to act!
    if (actionBar_.GetSafeHwnd())
    {
      if (actionBar_.GetDlgItem(ID_BTN_1) &&
          actionBar_.GetDlgItem(ID_BTN_1)->IsWindowEnabled())
      {
        // Player is being queried right now,
        // don't do anything
      }
      else
      {
        if (cmd == ID_TOGGLEACTIONBARDOCK)
        {
            if (!actionBar_.isDocked())
            {   // will dock

                // Action bar must be made horizontal in
                // order to dock
                int layout = Registry::GetActionBarLayout(0);
                Registry::SetActionBarLayout(1);

                if (layout == 0)
                {
                    recreateActionBar();
                    Registry::SetActionBarLayout(0);
                }
            }

            int dock = Registry::GetActionBarDock(DefaultActionBarDock);
            if (dock == 0)
                Registry::SetActionBarDock(1);
            else
                Registry::SetActionBarDock(0);

            actionBar_.dock(!dock);

            if (actionBar_.isDocked())
            {   // Make window higher so the table graphics is visible
                CRect rectWindow, rectClient;
                GetWindowRect(&rectWindow);
                GetClientRect(&rectClient);
                ClientToScreen(&rectClient);

                if (!Base::IsMaximized(this))//rectClient.Height() < 480)
                {                    
                    CRect rectABar(0, 0, 0, 0),
                          rectSBar(0, 0, 0, 0);
                    actionBar_.GetWindowRect(&rectABar);
                    rectWindow.bottom = rectWindow.top + 
                                        500 +
                                        rectABar.Height();
                    MoveWindow(&rectWindow);
                }
            }
        }
        else
        {
            // User toggle layout - must undock!
            Registry::SetActionBarDock(0);

            int layout = Registry::GetActionBarLayout(0);
            if (layout == 0)
                layout = 1;
            else
                layout = 0;
            Registry::SetActionBarLayout(layout);

            recreateActionBar();
        }
      }
    }

}

void CMainWnd::toggleChatWnd(int mode)
{
  Registry::SetChatWndDock(mode);

  if (mode != 0 && mode != 1 && mode != 2)
      mode = 1;

  chatDlg_.dock((CChatDlg::ChatDlg_DockMode)mode);

  if (chatDlg_.isDocked())
    chatDlg_.attach();
}

void CMainWnd::OnRequestprevhand() 
{
  CString s = GetHandLogEmailAddress(this);
    
  if (Global::TableServer() && !s.IsEmpty())
  {
    if (Global::TableServer()->sendSetupHandLogRequestPDU(0, (LPCSTR)s) != -1)
    {
      AfxMessageBox("Request for last hand log sent.", MB_OK|MB_ICONINFORMATION);
    }
    else
    {
      AfxMessageBox("Failed to send last hand log request.");
    }
  }
}

void CMainWnd::OnUpdateRequestprevhand(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(!Global::IsObserver() && Global::TableServer() != NULL);
}

void CMainWnd::OnRequest5handlog() 
{
  CString s = GetHandLogEmailAddress(this);
    
  if (Global::TableServer() && !s.IsEmpty())
  {
    if (Global::TableServer()->sendSetupHandLogRequestPDU(1, (LPCSTR)s) != -1)
    {
      AfxMessageBox("Request for last 5 hands log sent.", MB_OK|MB_ICONINFORMATION);
    }
    else
    {
      AfxMessageBox("Failed to send 5 last hand log request.");
    }
  }
}

void CMainWnd::OnUpdateRequest5handlog(CCmdUI* pCmdUI) 
{
  pCmdUI->Enable(!Global::IsObserver() && Global::TableServer() != NULL);
}


void CMainWnd::OnMove(int x, int y) 
{
  using Base::GameLogic;

  CFrameWnd::OnMove(x, y);

  if (GameLogic::Instance().currentState())
    GameLogic::Instance().currentState()->onWndMove(x, y);

  if (actionBar_.GetSafeHwnd() && actionBar_.isDocked())
  {
      actionBar_.attach();
  }

  if (chatDlg_.GetSafeHwnd() && chatDlg_.isDocked())
  {
      chatDlg_.attach();
  }
}

// Buying in more chips is implemented by sending
// a BuyInMore PDU. Server replies with a BuyIn
// Query PDU, and the actual buy-in is then handled
// in the same way as when logging in.
void CMainWnd::OnBuyin() 
{
  if (!CTableView::Instance()->canBuyIn())
  {
    AfxMessageBox("You need to sit out in order to buy more chips.", MB_ICONINFORMATION);
    return;
  }

  if (Global::TableServer())
  {
    Global::TableServer()->sendSetupBuyinMore();
  }
}

void CMainWnd::OnUpdateBuyin(CCmdUI* pCmdUI) 
{
  BOOL enable = !Global::IsObserver() && Global::TableServer() != NULL;

/*  if (enable)
  {
      // Player can buy in more only if he is sitting out
      enable = CTableView::Instance()->canBuyIn();
  }
*/
  pCmdUI->Enable(enable);
}

//#define TEST_LOGINLOGOUT_ 1

void CMainWnd::OnTimer(UINT nIDEvent) 
{    
    if (nIDEvent == ID_TIMER_HEARTBEAT)
    {
        if (!Global::CloseConnection() && Global::TableServer())
        {
            Global::TableServer()->sendHeartBeatPDU();
        }
    }

#ifdef TEST_LOGINLOGOUT_

    if (nIDEvent == ID_TIMER_TESTLOGINLOGOUT)
    {
        if (!Global::Instance().TableServer())
        {
            int i = Registry::GetInstanceNumber();
            CString s, p;
            s.Format("p%i", i);
            p.Format("p%i", i);
            // Log in
            Global::Instance().loginToTableServer("cardroom1.pokerspot.com",
                                                  8401,
                                                  s, p);
        }
        else
        {
            // Log out
            Base::GameLogic::LeaveTable(TRUE); 
            CTableView::Instance()->recreateLeaveTableButton(ID_CANCELLOGOUT);
        }
    }
    else if (nIDEvent == ID_TIMER_LOGOUTEXIT)
    {
        // Login/logout test mode: just delete the
        // logout exit timer
        KillTimer(nIDEvent);
    }
	
#else

	CFrameWnd::OnTimer(nIDEvent);

    if (nIDEvent == ID_TIMER_LOGOUTEXIT)
    {
        if (Global::TableServer() == NULL)
        {
            static int i = 2;
            if (i-- == 0)
            {
                KillTimer(nIDEvent);
                PostMessage(WM_CLOSE, 0, 0);
            }
        }
    }

#endif
}

void CMainWnd::OnTestloginout() 
{
#ifdef TEST_LOGINLOGOUT_
    int i = 60 + (rand() % 60);
    CString s;
    s.Format("Timer: %i", i);
    AfxMessageBox(s);

    SetTimer(ID_TIMER_TESTLOGINLOGOUT, i * 1000, NULL);
    OnTimer(ID_TIMER_TESTLOGINLOGOUT);
#endif
}


int CMainWnd::getStatusBarHeight() const
{
    if (statusBar_.GetSafeHwnd() &&
        statusBar_.IsWindowVisible())
    {
        CRect r;
        statusBar_.GetWindowRect(&r);
        return r.Height();
    }
    else
    {
        return 0;
    }
}


void CMainWnd::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
    // This window has only the owner draw menu,
    // forward this to the menu
    CMenu* pMenu = GetMenu();
    if (pMenu)
        pMenu->MeasureItem(lpMeasureItemStruct);
}
