/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CPokerApp
  Created  : 25.10.1999

  OVERVIEW : CPokerApp is the CWinApp derivative class.

 ****************************************************************************/

#include "stdafx.h"
#include <afxsock.h>
#include "poker.h"
#include "mainwnd.h"
#include "ui/global.h"
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

  LPCTSTR g_szWndClass = _T("PSILoungeWnd");
  LPCTSTR g_szAppName  = _T("PokerSpot Lounge");
  LPCTSTR g_szFontFace = _T("Arial");
  LPCTSTR g_szCantFindTableClient = 
    _T("The table client 'table.exe' must "
       "reside in the same directory as the "
       "lounge client.\n\nPlease correct the problem "
       "and restart the lounge client.");

  LPCTSTR g_szNotEnoughMemory = "Not enough memory to run!";

  const int g_FontSize = 9;

  // Max ticks between frames 5 seconds
  const DWORD MaxTicksPerFrame = 5000;

  // The main app object.
  CPokerApp g_App;
}

namespace
{
  void SanityCheckMax(DWORD& d, const DWORD dwMax)
  {
    if (d > dwMax)
      d = dwMax;
  }
}


/////////////////////////////////////////////////////////////////////////////
// CPokerApp

BEGIN_MESSAGE_MAP(CPokerApp, CWinApp)
  //{{AFX_MSG_MAP(CPokerApp)
    // NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPokerApp construction

int CPokerApp::iCharHeight_ = 0;

CPokerApp::CPokerApp()
  :
  CWinApp(g_szAppName)
{
}

/////////////////////////////////////////////////////////////////////////////
// CPokerApp MFC Overrides


//
// Override InitInstance to create main window.
//
BOOL CPokerApp::InitInstance()
{
  if (!AfxSocketInit())
  {
    AfxMessageBox(IDS_SOCKETINITFAILED);
    return FALSE;
  }
  
  CMainWnd* pMainWnd = new CMainWnd();
  m_pMainWnd = pMainWnd;

  BOOL rc = FALSE;

  if (pMainWnd)
  {
    rc = pMainWnd->Create(NULL, g_szWndClass);
  }
  else
  {
    rc = FALSE;
    AfxMessageBox(g_szNotEnoughMemory);
  }

  if (rc)
    rc = Global::Instance().initInstance();

  return rc;
}


BOOL CPokerApp::OnIdle(LONG lCount) 
{
  return CWinApp::OnIdle(lCount);
}


//
// If we need faster frame rate, we could roll
// our own message pump in CPokerApp::Run() and
// save the MFC OnIdle overhead.
//
// For this particular application, the OnIdle
// overhead does not matter so we let the default
// implementation do its job.
//
int CPokerApp::Run() 
{
  return CWinApp::Run();
}


/////////////////////////////////////////////////////////////////////////////
// CPokerApp application mfunctions


LPCTSTR CPokerApp::AppName()
{
  return g_szAppName;
}


LPCTSTR CPokerApp::GetFontFace()
{
  return g_szFontFace;
}


CFont* CPokerApp::GetFont()
{
  return g_App.getFont();
}


CFont* CPokerApp::GetBoldFont()
{
  return g_App.getBoldFont();
}


CFont* CPokerApp::getFont() const
{
  CPokerApp* ncThis = const_cast<CPokerApp*>(this);

  if (!font_.GetSafeHandle())
  {
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    ncThis->font_.CreateFont(-((g_FontSize * GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY)) / 72),
                             0, 0, 0, FW_NORMAL, 0, 0, 0, 0, 0, 0, 0, 0,
                             CPokerApp::GetFontFace());
  }

  return &(ncThis->font_);
}


CFont* CPokerApp::getBoldFont() const
{
  CPokerApp* ncThis = const_cast<CPokerApp*>(this);

  if (!fontBold_.GetSafeHandle())
  {
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    ncThis->fontBold_.CreateFont(-((g_FontSize * GetDeviceCaps(dc.GetSafeHdc(), LOGPIXELSY)) / 72),
                                 0, 0, 0, FW_BOLD, 0, 0, 0, 0, 0, 0, 0, 0,
                                 CPokerApp::GetFontFace());
  }

  return &(ncThis->fontBold_);
}

int CPokerApp::GetCharHeight()
{
  if (iCharHeight_ == 0)
  { // First call computes the char height.
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CFont* pOldFont = dc.SelectObject(CPokerApp::GetFont());
    TEXTMETRIC tm;
    ::ZeroMemory(&tm, sizeof(tm));
    dc.GetTextMetrics(&tm);
    dc.SelectObject(pOldFont);
    iCharHeight_ = tm.tmHeight;
  }
  return iCharHeight_;
}


CPokerApp* CPokerApp::Instance()
{
  return &g_App;
}


int CPokerApp::ExitInstance() 
{
  Global::Instance().exitInstance();

#ifdef _DEBUG
  // Detect PDU memory leaks
  TRACE1("PDU Allocation Balance: %d\n", Network::PDUHeader::AllocBalance_);
#endif

  return CWinApp::ExitInstance();
}

#if 0 // These have been moved to class Global
bool CPokerApp::loginToLoungeServer(const CString& host,
                                    int            port,
                                    const CString& username,
                                    const CString& passwd)
{
  CWaitCursor wait;

  CView* pView = static_cast<CMainWnd*>(m_pMainWnd)->GetActiveView();

  if (!pLoungeServer_)
    pLoungeServer_ = Network::LoungeServer::ConnectToLounge(pView ? pView->GetSafeHwnd() : NULL,
                                                            (LPCTSTR)host, port);

  if (pLoungeServer_)
    pLoungeServer_->sendLoungeLoginPDU(username, passwd);

  return pLoungeServer_ != NULL;
}


void CPokerApp::closeLoungeServer()
{
  if (pLoungeServer_)
    pLoungeServer_->closeConnection();
  delete pLoungeServer_;
  pLoungeServer_ = 0;
}


bool CPokerApp::loginToTableServer(UINT           host,
                                   int            port,
                                   const CString& username,
                                   const CString& passwd)
{
  // Lounge client does not log in to the table
  return false;
}


bool CPokerApp::loginToTableServer(const CString& host,
                                   int            port,
                                   const CString& username,
                                   const CString& passwd)
{
  // Lounge client does not log in to the table
  return false;
}


void CPokerApp::closeTableServer()
{
  // Lounge client does not log in to the table
}


void CPokerApp::closeConnections()
{
  Network::Server::Quit();
  closeLoungeServer();
}
#endif // Move to class Global