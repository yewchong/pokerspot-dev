/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CMainWnd
  Created  : 01/14/2000

  OVERVIEW : CMainWnd is the main window class derived
             from CFrameWnd.

 ****************************************************************************/

#include "stdafx.h"
#include "lounge/mainwnd.h"
#include "lounge/floormapview.h"
#include "ui/global.h"
#include "ui/chatview.h"
#include "ui/logindlg.h"
#include "ui/motddlg.h"
#include "ui/registry.h"
#include "ui/soundmanager.h"
#include "ui/cmdid.h"
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
  LPCTSTR g_szConfirmLogout = _T("Are you sure you want to log out?");
  
  //
  // Module functions
  //

BOOL IsMaximized(CWnd* pWnd)
{
  WINDOWPLACEMENT wpml;
  ::ZeroMemory(&wpml, sizeof(wpml));
  wpml.length = sizeof(wpml);

  pWnd->GetWindowPlacement(&wpml);
  return wpml.showCmd == SW_MAXIMIZE;
}


} // namespace


CMainWnd* CMainWnd::pInstance_ = 0;


CMainWnd::CMainWnd()
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
  ON_COMMAND(ID_FLOORMAP, OnFloormap)
  ON_WM_GETMINMAXINFO()
  ON_COMMAND(ID_SOUNDTOGGLE, OnSoundtoggle)
  ON_UPDATE_COMMAND_UI(ID_SOUNDTOGGLE, OnUpdateSoundtoggle)
  //}}AFX_MSG_MAP
  ON_UPDATE_COMMAND_UI(ID_INDICATOR_ONLINE, OnUpdateOnlineIndicator)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMainWnd message handlers

BOOL CMainWnd::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName)
{
  BOOL rc = CFrameWnd::Create(lpszClassName,
                              Global::AppName());

  if (rc)
  {
    restoreSettings();
    chatDlg_.ShowWindow(SW_SHOWNORMAL);

    // Start with login dialog
    PostMessage(WM_COMMAND, MAKEWPARAM(ID_LOGIN, 0), 0);
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
// the child view.
//
BOOL CMainWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
  CView* pView = new CFloorMapView();
  if (pView)
  {
    if (pView->Create(NULL, NULL, WS_CHILD, //AFX_WS_DEFAULT_VIEW,
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
// Override OnCreate to create the chat windows.
int CMainWnd::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

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
  //statusBar_.SetPaneInfo(1, 0, SBPS_NORMAL, 50);
  statusBar_.SetPaneText(1, "", TRUE);
#endif

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
// Override OnSize to adjust the splitter.
//
void CMainWnd::OnSize(UINT nType, int cx, int cy) 
{
  CFrameWnd::OnSize(nType, cx, cy);
}


void CMainWnd::OnClose() 
{
  if (AfxMessageBox(g_szConfirmQuit, MB_ICONQUESTION|MB_YESNO) == IDYES)
  {
    CMenu* pMenu = GetMenu();
    delete pMenu;
    SetMenu(0);
    CFrameWnd::OnClose();

    if (Global::LoungeServer())
      Global::LoungeServer()->sendLoungeLogoutPDU();
    Network::Server::Quit();
  } 
}


void CMainWnd::OnAppAbout() 
{
}


void CMainWnd::OnLogin() 
{
  CLoginDlg dlg(this);
  dlg.DoModal();
}


void CMainWnd::OnUpdateLogin(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(Global::LoungeServer() == NULL);
}


void CMainWnd::OnLogout() 
{
  if (AfxMessageBox(g_szConfirmLogout, MB_ICONQUESTION|MB_YESNO) == IDYES)
  {
    if (Global::LoungeServer())
      Global::LoungeServer()->sendLoungeLogoutPDU();
  }
}


void CMainWnd::OnUpdateLogout(CCmdUI* pCmdUI)
{
  pCmdUI->Enable(Global::LoungeServer() != NULL);
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
  
  if (IsMaximized(this))
    r.right = r.bottom = MW_Maximized;
  else
    GetWindowRect(&r);
  Registry::SetWndRect(r);

  if (::IsWindow(chatDlg_.GetSafeHwnd()))
  {
    chatDlg_.GetWindowRect(&r);
    Registry::SetChatRect(r);
  }
}


//
// Restore window state from registry.
//
void CMainWnd::restoreSettings()
{
  CRect r(0, 0, -1, -1);
  r = Registry::GetWndRect(r);

  CRect rectBar;
  chatDlg_.GetClientRect(&rectBar);
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
    { // Make sure action bar has enough room
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

  r = Registry::GetChatRect(rectBar);
  chatDlg_.SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(),
                        SWP_NOZORDER);
}


void CMainWnd::OnFloormap() 
{  
  CView* pView = GetActiveView();
  if(!pView) return;
  if (pView && pView->IsKindOf(RUNTIME_CLASS(CFloorMapView)))
    return;

  pView->DestroyWindow();

  pView = new CFloorMapView();
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
  if (Global::LoungeServer())
  {
    CString s;
    s.LoadString(ID_INDICATOR_ONLINE);
    pCmdUI->SetText(s);
    pCmdUI->Enable(TRUE);
  }
  else
    pCmdUI->Enable(FALSE);
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