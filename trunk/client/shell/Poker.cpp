/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CPokerApp
  Created  : 25.10.1999

  OVERVIEW : CPokerApp is the CWinApp derivative class.

 ****************************************************************************/

#include "stdafx.h"
#include <afxsock.h>
#include "poker.h"
#include "network/ssl.h"
#include "interface/igameimplementation.h"
#include "interface/igamedispatch.h"
#include "ui/global.h"
#include "ui/tableview.h"
#include "ui/cmdid.h"
#include "ui/splashwnd.h"
#include "ui/registry.h"
#include "base/cipher.h"
#include "base/misc.h"
#include "common/chips.h"

#ifdef NO_TABLEIMPL_DLL_
#include "ui/mainwnd.h"
#include "base/gamelogic.h"
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif



BOOL CALLBACK EnumWndFunc(HWND hWnd, LPARAM lParam)
{
  CWnd* pChild = CWnd::FromHandle(hWnd);
  if (pChild)
  {
    CString s;
    pChild->GetWindowText(s);

    if (s.Find("PokerSpot Table -") != -1)
    { // it's holdem client
      int* pint = reinterpret_cast<int*>(lParam);
      (*pint)++;
    }
  }
  return TRUE;
}


// Return the number of table client instances
// already running.
int CountInstances()
{
  int rc = 0;

  // enumerate windows, inspect caption
  ::EnumWindows(EnumWndFunc,
                reinterpret_cast<LPARAM>(&rc));

  return rc;
}


// Link in the multimedia library for timeGetTime
#pragma comment (lib, "winmm.lib")

// This is the game DLL entry point function prototype.
typedef HRESULT (PASCAL *DllGetGameImpl)(IGameImplementation**);

namespace
{
  // Module globals

#ifdef NO_TABLEIMPL_DLL_
  LPCTSTR g_szWndClass = _T("PSITableWnd");
  LPCTSTR g_szAppName  = _T("PokerSpot Table");
  LPCTSTR g_szNotEnoughMemory = _T("Not enough memory to run!");
#endif

  // Max ticks between frames 5 seconds
  const DWORD MaxTicksPerFrame = 5000;

  // The main app object.
  CPokerApp g_App;

  HINSTANCE g_hDll = NULL;

  // Entry points to the game implementation
  IGameImplementation* g_pGameImpl = NULL;
  IGameDispatch* g_pGameDisp = NULL;

  LPCTSTR g_szInitSocketFailed =
    _T("Initialization of the Sockets library failed.");
  LPCTSTR g_szCantLoadGameDll = 
    _T("Unable to load the game DLL!\n\n"
       "The game DLL must be located in the same directory "
       "as the game executable.");
  LPCTSTR g_szInvalidDll =
    _T("The game DLL is corrupted. Please contact PSI support "
       "to obtain a new game DLL.");
  LPCTSTR g_szCantInitSSL = _T("SSL initialization failed.");
}

namespace
{

void SanityCheckMax(DWORD& d, const DWORD dwMax)
{
  if (d > dwMax)
    d = dwMax;
}

BOOL LoadGameInterfaces(HMODULE hDll)
{
  BOOL rc = FALSE;

  // Get the game entry point function
  DllGetGameImpl pFunc = (DllGetGameImpl)::GetProcAddress(hDll,
                                             "DllGetGameImplementation");
  if (pFunc)
  {
    // Get the IGameImplementation
    (*pFunc)(&g_pGameImpl);

    if (g_pGameImpl)
    { 
      // We have the game implementation -
      // get the IGameDispatch
      rc = SUCCEEDED(g_pGameImpl->GetGameDispatch(&g_pGameDisp)) &&
           g_pGameDisp != NULL;
    }
  }

  return rc;
}


HWND g_LoungeHWND = 0;
//USHORT g_TableNumber = 0;
//UINT g_LogoutMsg = 0;

void DoCommandLine(LPCTSTR cmdLine)
{
  CString launchData;

  TRY
  {
    CFile f(cmdLine, CFile::modeRead);
    f.SeekToEnd();
    DWORD size = f.GetPosition();
    f.SeekToBegin();
    char* buf = new char[size + 1];
    if (buf)
    {
      f.Read(buf, size);
      f.Close();
      buf[size] = 0;
      launchData = buf;
      delete [] buf;
    }
    else
    {
      Base::SafeRemove(cmdLine);
      AfxMessageBox("Out of memory!");
      return;
    }
  }
  CATCH (CFileException, e)
  {
    Base::SafeRemove(cmdLine);
    TCHAR szCause[255];
    e->GetErrorMessage(szCause, 255);
    CString s;
    s.Format("Unable to run (%s). Please retry joining the table!", szCause);
    AfxMessageBox(s);
    return;
  }
  END_CATCH

  Base::SafeRemove(cmdLine);

  // Parse the command line and do an automatic
  // log in to the designated server
  CString obs, gameType, hilo, username, pw,
          host, port, 
          lolimit("3"), hilimit("6"),
          hwnd, tablenumber;

  int len = strlen(launchData);
  char* tmp = new char[len + 1];
  strcpy(tmp, launchData);
  tmp[len] = '\0';
  const char* tn = NULL;

  obs = strtok(tmp, ":");
  gameType = strtok(NULL, ":");
  hilo = strtok(NULL, ":");
  username = strtok(NULL, ":");
  host = strtok(NULL, ":");
  port = strtok(NULL, ":");
  lolimit = strtok(NULL, ":");
  hilimit = strtok(NULL, ":");
  hwnd = strtok(NULL, ":");
  tn = strtok(NULL, ":");
  tablenumber = tn;
  // The last parameter is encrypted password
  if (tn != NULL && *tn != '\0')
  {
    // point after tablenumber
    pw = tn + strlen(tn) + 1;
  }

  delete [] tmp;
  tmp = 0;

  Global::SetObserver(obs == "1");
  Global::SetGameType((GameType)atoi(gameType));

  ASSERT(username.GetLength() > 0 && 
         pw.GetLength() > 0 &&
         host.GetLength() > 0 &&
         port.GetLength() > 0 &&
         lolimit.GetLength() > 0 &&
         hilimit.GetLength() > 0);

  if (username.GetLength() > 0 && 
      pw.GetLength() > 0 &&
      host.GetLength() > 0 &&
      port.GetLength() > 0)
  {
    CString password = pw;

    ULONG h = atoi(host);
    USHORT p = atoi(port);
    CChips lo = atof(lolimit);
    CChips hi = atof(hilimit);

    if (CTableView* table = CTableView::Instance())
    {
      table->lolimit(lo);
      table->hilimit(hi);
      table->setIsHiLo((atoi(hilo) == 0) ? false : true);
      table->setTableNumber(atoi(tablenumber));
      table->setCaption();
    }
    
    Global::Instance().loginToTableServer(h, p,
                                          username,
                                          password);
  }

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
  //g_LogoutMsg = ::RegisterWindowMessage("PokerSpot Logout Message");

  Registry::SetInstanceNumber(CountInstances() + 1);

  Base::GameLogic::TakeFocusOnRequest(Registry::GetSettingsTakeFocus(1));

  BOOL got_cmdline = lstrlen(m_lpCmdLine);
  // Don't show splash screen if we have command line
  CSplashWnd::EnableSplashScreen(!got_cmdline);

#ifdef NO_TABLEIMPL_DLL_
  // Initialize as stand-alone EXE

  if (!AfxSocketInit())
  {
    AfxMessageBox(g_szInitSocketFailed);
    return FALSE;
  }

  // Initialize SSL crypting library
  BOOL rc = PSI_SSL_Init();
  if (!rc)
  {
    AfxMessageBox(g_szCantInitSSL);
    return FALSE;
  }

  CMainWnd* pMainWnd = new CMainWnd(0);
  m_pMainWnd = pMainWnd;

  if (pMainWnd)
  {
    rc = pMainWnd->Create(!got_cmdline, NULL, g_szWndClass);
  }
  else
  {
    rc = FALSE;
    AfxMessageBox(g_szNotEnoughMemory);
  }

  if (rc)
  {
    rc = Global::Instance().initInstance();
    if (got_cmdline)
      DoCommandLine(m_lpCmdLine);
  }

  return rc;

#else
  // EXE-DLL initialization

#pragma error("EXE-DLL initialization not implemented!")

  // Call DLL entry point function to
  // initialize the application and to
  // create application main window
  g_hDll = ::LoadLibrary("tableimpl.dll");

  if (!g_hDll)
  {
    AfxMessageBox(g_szCantLoadGameDll);
    return FALSE;
  }

  if (!LoadGameInterfaces(g_hDll))
  {
    ::FreeLibrary(g_hDll);
    g_hDll = NULL;
    AfxMessageBox(g_szInvalidDll);
    return FALSE;
  }

  // Initialize crypting library
  BOOL rc = PSI_SSL_Init();
  if (!rc)
  {
    AfxMessageBox(g_szCantInitSSL);
    return FALSE;
  }

  if (!AfxSocketInit())
  {
    ::FreeLibrary(g_hDll);
    g_hDll = NULL;
    AfxMessageBox(g_szInitSocketFailed);
    return FALSE;
  }

  CWnd* pMainWnd = NULL;
  g_pGameImpl->CreateMainWindow(&pMainWnd);

  m_pMainWnd = pMainWnd;

  return (pMainWnd != NULL); 

#endif
}


//
// Override OnIdle to do our game hearbeat.
//
BOOL CPokerApp::OnIdle(LONG lCount) 
{
  BOOL rc = CWinApp::OnIdle(lCount);

#ifndef NO_TABLEIMPL_DLL_
  // Call into the DLL's implementation for heartbeat

  // Obtain ptr to IGameLogic from DLL,
  // call its Tick function
  if (!rc)
  { // MFC is done, do our game heartbeat
    static DWORD prevTime = ::timeGetTime();
    DWORD curTime = ::timeGetTime();
    DWORD dt = curTime - prevTime;

    if (dt == 0) return TRUE;

    SanityCheckMax(dt, MaxTicksPerFrame);

    prevTime = curTime;

    if (g_pGameDisp)
    {
      // Let game logic do one heartbeat
      g_pGameDisp->Tick(dt, &rc);
    }
  }

#else
  // Stand-alone exe
  if (!rc)
  { // MFC is done, do our game heartbeat
    
    static DWORD prevTime = ::timeGetTime();
    DWORD curTime = ::timeGetTime();
    DWORD dt = curTime - prevTime;

    if (dt == 0) return TRUE;

    SanityCheckMax(dt, MaxTicksPerFrame);

    prevTime = curTime;

    // Let game logic do one heartbeat
    rc = Base::GameLogic::Instance().tick(dt);
  }

#endif

  return rc;
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


int CPokerApp::ExitInstance() 
{
  Global::Instance().exitInstance();

  if (g_hDll != NULL)
  {
    ::FreeLibrary(g_hDll);
    g_hDll = NULL;
  }

  PSI_SSL_Cleanup();

  // Tell the lounge client we're going...  
  /*::PostMessage(g_LoungeHWND,
                g_LogoutMsg,
                MAKEWPARAM(g_TableNumber, 0), 0);*/

  return CWinApp::ExitInstance();
}


BOOL CPokerApp::PreTranslateMessage(MSG* pMsg) 
{
  if (CSplashWnd::PreTranslateAppMessage(pMsg))
    return TRUE;

  return CWinApp::PreTranslateMessage(pMsg);
}
