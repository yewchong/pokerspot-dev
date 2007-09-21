// Lounge.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "lounge.h"
#include "loungedlg.h"
#include "registry.h"
#include "ui/global.h"
#include "ui/splashwnd.h"
#include "network/ssl.h"
#include "base/md5.h"
#include "base/misc.h"


#pragma comment(lib, "winmm.lib")

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
  LPCTSTR g_szTitle = _T("PokerSpot Lounge");
  LPCTSTR g_szCantInitSSL = _T("SSL initialization failed.");
  LPCTSTR g_szCantInitSockets = _T("Windows sockets initialization failed.");

  // Helper routine to remove temporary launch data
  // files, just in case
  void RemoveTemporaryFiles()
  {
    const char* s = "ldf*.*";
    WIN32_FIND_DATA data;
    memset(&data, 0, sizeof(data));

    HANDLE h = FindFirstFile(s, &data);
    if (h == INVALID_HANDLE_VALUE)
      return;

    BOOL done = FALSE;
    while (!done)
    {
      DWORD attrib = GetFileAttributes(data.cFileName);
      if (!(attrib & FILE_ATTRIBUTE_DIRECTORY))
      {
        Base::SafeRemove(data.cFileName);
      }
      done = !FindNextFile(h, &data);
    }
  }
}


BEGIN_MESSAGE_MAP(CLoungeApp, CWinApp)
  //{{AFX_MSG_MAP(CLoungeApp)
  	// NOTE - the ClassWizard will add and remove mapping macros here.
	//    DO NOT EDIT what you see in these blocks of generated code!
  //}}AFX_MSG
  ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoungeApp construction

CLoungeApp::CLoungeApp()
  :
  CWinApp(g_szTitle)
{}

/////////////////////////////////////////////////////////////////////////////
// The one and only CLoungeApp object

CLoungeApp g_theApp;

/////////////////////////////////////////////////////////////////////////////
// CLoungeApp initialization

BOOL CLoungeApp::InitInstance()
{
  RemoveTemporaryFiles();

  if (!AfxSocketInit())
  {
    AfxMessageBox(g_szCantInitSockets);
    return FALSE;
  }

  // Check that the client configuration file
  // is available - without it we can't do much
  u_byte_t checksum[16];
  memset(checksum, 0, sizeof(checksum));
  if (Base::ComputeMD5Checksum("client_versions.conf", checksum) == -1)
  {
    // Cannot open the configuration file!!!
    // Will send 0 as the checksum and the new file
    // will automatically downloaded.
    AfxMessageBox("The client configuration file \"client_versions.conf\" was not "
                  "found. This file must be located in the PokerSpot client installation directory.");
    return FALSE;
  }

  // Initialize crypting library
  BOOL rc = PSI_SSL_Init();
  if (!rc)
  {
    AfxMessageBox(g_szCantInitSSL);
    return FALSE;
  }

  CSplashWnd::EnableSplashScreen(TRUE);

  // Standard initialization
  // If you are not using these features and wish to reduce the size
  //  of your final executable, you should remove from the following
  //  the specific initialization routines you do not need.

#ifdef _AFXDLL
  Enable3dControls();		// Call this when using MFC in a shared DLL
#else
  Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

  CLoungeDlg dlg;
  m_pMainWnd = &dlg;

  restoreSettings();

  int nResponse = dlg.DoModal();

  if (nResponse == IDOK)
  { // XXX NOTE: property sheet does not return
    // IDOK when "Finish" button is clicked but
    // something else.
  }
  else if (nResponse == IDCANCEL)
  {
  }

  saveSettings();

  Global::Instance().closeLoungeServer();
  PSI_SSL_Cleanup();

  // Since the dialog has been closed, return FALSE so that we exit the
  //  application, rather than start the application's message pump.
  return FALSE;
}


void CLoungeApp::saveSettings()
{
}


void CLoungeApp::restoreSettings()
{
}


BOOL CLoungeApp::PreTranslateMessage(MSG* pMsg) 
{
  if (CSplashWnd::PreTranslateAppMessage(pMsg))
    return TRUE;

  return CWinApp::PreTranslateMessage(pMsg);
}

int CLoungeApp::ExitInstance() 
{
  RemoveTemporaryFiles();	
  return CWinApp::ExitInstance();
}
