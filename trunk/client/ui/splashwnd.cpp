/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CSplashWnd
  Created  : 03/10/2000

  OVERVIEW : Implements a splash screen window.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/splashwnd.h"
#include "ui/cmdid.h"
#include "base/jpgimage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
//   Splash Screen class

BOOL CSplashWnd::c_bShowSplashWnd;
CSplashWnd* CSplashWnd::c_pSplashWnd;
CSplashWnd::CSplashWnd()
  :
  pImage_    (0),
  postLogin_(FALSE),
  quitOnTimer_(TRUE)
{}

CSplashWnd::~CSplashWnd()
{
  delete pImage_;
  pImage_ = 0;

  // Clear the static window pointer.
  ASSERT(c_pSplashWnd == this);
  c_pSplashWnd = NULL;
}

BEGIN_MESSAGE_MAP(CSplashWnd, CWnd)
  //{{AFX_MSG_MAP(CSplashWnd)
  ON_WM_CREATE()
  ON_WM_PAINT()
  ON_WM_TIMER()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CSplashWnd::EnableSplashScreen(BOOL bEnable /*= TRUE*/)
{
  c_bShowSplashWnd = bEnable;
}

BOOL CSplashWnd::ShowSplashScreen(LPCTSTR imagefile,
                                  BOOL postLogin,
                                  BOOL quitOnTimer,
                                  CWnd* pParentWnd /*= NULL*/)
{
  if (!c_bShowSplashWnd || c_pSplashWnd != NULL)
    return FALSE;

  // Allocate a new splash screen, and create the window.
  c_pSplashWnd = new CSplashWnd;
  BOOL rc = c_pSplashWnd->Create(imagefile,
                                 postLogin,
                                 quitOnTimer,
                                 pParentWnd);
  if (!rc)
  {
    delete c_pSplashWnd;
    c_pSplashWnd = 0;
  }
  else
  {
      if (pParentWnd && !pParentWnd->IsWindowVisible())
      {
        CRect splash, r;
        CSplashWnd::c_pSplashWnd->GetWindowRect(&splash);
        pParentWnd->GetWindowRect(&r);

        CSplashWnd::c_pSplashWnd->MoveWindow(r.left + r.Width() / 2 - splash.Width() / 2,
                                             r.top + r.Height() / 2 - splash.Height() / 2,
                                             splash.Width(), splash.Height());
      }
      else if (pParentWnd)
      {
        c_pSplashWnd->CenterWindow(pParentWnd);
      }
      c_pSplashWnd->UpdateWindow();
  }

  return rc;
}

BOOL CSplashWnd::PreTranslateAppMessage(MSG* pMsg)
{
  if (c_pSplashWnd == NULL)
    return FALSE;

  // If we get a keyboard or mouse message, hide the splash screen.
  if (pMsg->message == WM_KEYDOWN ||
      pMsg->message == WM_SYSKEYDOWN ||
      pMsg->message == WM_LBUTTONDOWN ||
      pMsg->message == WM_RBUTTONDOWN ||
      pMsg->message == WM_MBUTTONDOWN ||
      pMsg->message == WM_NCLBUTTONDOWN ||
      pMsg->message == WM_NCRBUTTONDOWN ||
      pMsg->message == WM_NCMBUTTONDOWN)
  {
    c_pSplashWnd->HideSplashScreen();
    return TRUE;	// message handled here
  }

  return FALSE;	// message not handled
}

BOOL CSplashWnd::Create(LPCTSTR imagefile,
                        BOOL postLogin,
                        BOOL quitOnTimer,
                        CWnd* pParentWnd /*= NULL*/)
{
  pImage_ = Base::JPGImage::LoadImage(imagefile);
  if (!pImage_)
    return FALSE;

  postLogin_ = postLogin;
  quitOnTimer_ = quitOnTimer;

  CSize s = pImage_->getSize();

  BOOL rc = CreateEx(0,
		     AfxRegisterWndClass(0, AfxGetApp()->LoadStandardCursor(IDC_ARROW)),
		     NULL, WS_POPUP | WS_EX_TOPMOST | WS_VISIBLE, 0, 0, s.cx, s.cy, pParentWnd->GetSafeHwnd(), NULL);

  return rc;
}

void CSplashWnd::HideSplashScreen()
{
  if (postLogin_)
  {
    // Start the app with login screen
    AfxGetMainWnd()->PostMessage(WM_COMMAND,
                                 MAKEWPARAM(ID_LOGIN, 0),
                                 0);
  }

  // Destroy the window, and update the mainframe.
  DestroyWindow();
  AfxGetMainWnd()->UpdateWindow();
}

void CSplashWnd::PostNcDestroy()
{
  // Free the C++ class.
  delete this;
}

int CSplashWnd::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
  if (CWnd::OnCreate(lpCreateStruct) == -1)
    return -1;

  // Center the window.
  CenterWindow();

  if (quitOnTimer_)
  {
    // Set a timer to destroy the splash screen.
    SetTimer(1, 3000, NULL);
  }

  return 0;
}

void CSplashWnd::OnPaint()
{
  CPaintDC dc(this);

  if (pImage_)
    pImage_->drawImage(dc);
}

void CSplashWnd::OnTimer(UINT nIDEvent)
{
  // Destroy the splash screen window.
  HideSplashScreen();
}
