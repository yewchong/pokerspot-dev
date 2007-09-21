/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : pdulounge.cpp
  Created  : 10.11.1999

  OVERVIEW : Lounge PDUs.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdulounge.h"
#include "network/server.h"
#include "base/gamelogic.h"
#include "base/cipher.h"
#include "base/misc.h"
#include "ui/cmdid.h"
#include "ui/motddlg.h"
#include "ui/global.h"
#ifndef PSPOT_TABLE_MODULE_
  #include "base/autoupgrade.h"
  #include "loungedlg/registry.h"
  #include "loungedlg/floormap.h"
#endif
#include "common/stringoutput.h"

namespace
{
  LPCTSTR g_szConfirmFmt    = _T("Join table %d ($%d/$%d)?");
  LPCTSTR g_szLogoutWarning = _T("You are still playing in a table. Log out anyway?");
  LPCTSTR g_szMotd          = _T("PokerSpot Message of the Day");
  LPCTSTR g_szFailedToLaunchTable = _T("Failed to launch the table client.\n\n"
                                       "The table client 'holdem.exe' must reside in the "
                                       "same directory as the lounge client 'lounge.exe'");
  LPCTSTR g_szInvalidNameOrPassword = _T("Invalid username or password.\n\nRetry?");
  LPCTSTR g_szLoginFailed = _T("Login failed.\n\nPlease contact support@pokerspot.com if you have forgotten your password.");

CString MakeTmpFileName()
{
  char tmpName[MAX_PATH];

  if (!::GetTempFileName(".", "ldf", 0, tmpName))
    return CString("ldf1.tmp");
  else
    return CString(tmpName);
}


BOOL LaunchTable(const CString& username,
                 const CString& pw,
                 u_int16_t gameType,
                 u_int16_t isHiLo,
                 u_int16_t table,
                 const CChips& lolimit, const CChips& hilimit,
                 u_int32_t host, u_int16_t port,
                 HWND hWnd,
                 const char* observing = "0")
{
  static ULONG launch_id = 1;

  CString password = pw;/*do_cipher(pw,
                               pw.GetLength(),
                               CO_Encrypt);*/

  char buf0[32], bufhl[4],
       buf1[32], buf2[32],
       buf5[32], buf6[32];

  CString launchData;
  int dec = 0, sign = 0;
  double low = lolimit.asDouble(),
         high = hilimit.asDouble();
  CStrOut str_lolimit, str_hilimit;
  str_lolimit << lolimit.asDouble();
  str_hilimit << hilimit.asDouble();

  launchData = CString(observing) + ":" +
                 itoa(gameType, buf0, 10) + ":" +
                 itoa(isHiLo, bufhl, 10) + ":" +
                 username + ":" +  
                 itoa(host, buf1, 10) + ":" +
                 itoa(port, buf2, 10) + ":" +
                 str_lolimit.str() + ":" +
                 str_hilimit.str() + ":" +
                 itoa((int)hWnd, buf5, 10) + ":" +
                 itoa(table, buf6, 10) + ":" +
                 password;

  // Write the launch data to a temporary file,
  // and pass the file name on the command line
  CFile f;
  CString name = MakeTmpFileName();
  
  TRY
  {
    CFile f(name, CFile::modeCreate|CFile::modeWrite);
    f.Write(launchData, launchData.GetLength());
    f.Close();
  }
  CATCH (CFileException, e)
  {
    TCHAR szCause[255];
    e->GetErrorMessage(szCause, 255);
    CString s;
    s.Format("Unable to create temporary file: %s", szCause);
    AfxMessageBox(s);
    Base::SafeRemove(name);
    return FALSE;
  }
  END_CATCH

  SHELLEXECUTEINFO info;
  ::ZeroMemory(&info, sizeof(info));
  info.cbSize = sizeof(info);
  info.lpFile = "holdem.exe";
  info.lpVerb = "open";
  info.lpParameters = name;
  info.nShow = SW_SHOWNORMAL;

  BOOL success = ::ShellExecuteEx(&info);

  if (!success)
  {
    Base::SafeRemove(name);
    AfxMessageBox(g_szFailedToLaunchTable);
  }

  return success;
}

}



namespace Network
{

using Base::GameLogic;

// 
// The lounge client module sees these PDUs,
// the table client has dummy versions.
//
#ifdef PSPOT_TABLE_MODULE_

void PDULoungeAccept::execute(Base::GameState*) {}
void PDULoungeReject::execute(Base::GameState*) {}
void PDULoungeStatus::execute(Base::GameState*) {}
void PDULoungeTableFull::execute(Base::GameState*) {}
void PDULoungeSitting::execute(Base::GameState*) {}
void PDULoungeLogoutWarning::execute(Base::GameState*) {}
void PDULoungeLogoutConfirmed::execute(Base::GameState*) {}

#else

void PDULoungeAccept::execute(Base::GameState*)
{
  //
  // At this point, check if we need auto-upgrade
  // This can happen if previous auto-upgrade was
  // interrupted for some reason.
  //
  if (Registry::GetAutoUpgradeEnabled(1))
  {
    Base::ConfigEntries need_upgrade;
    Base::GetUpgradeModules("client_versions.conf", need_upgrade);
    if (need_upgrade.size() > 0)
    {      
      // Start the download
      CString upgWiz = Registry::GetUpgradeWizard("upgwiz2.exe");
      int rc = (int)ShellExecute(NULL, "open", upgWiz, NULL, NULL, SW_SHOWNORMAL);
    
      if (rc <= 32)
      { 
        AfxMessageBox("Unable to run Upgrade Wizard.\n\nPlease reinstall PokerSpot client. "
                      "Automatic upgrade will bring your installation up to date the next time you log in.");
      }
      AfxGetMainWnd()->PostMessage(WM_CLOSE, 0, 0);

      return;
    }
  }

  //
  // Ok to continue. We know now that the client
  // configuration file is up to date and that the
  // client binaries are the ones specified in the
  // configuration file.
  //
  
  Network::Server::Username(MakeString(username_));
  Network::Server::Password(MakeString(password_));

  CWnd* pMainWnd = AfxGetMainWnd();

  if (pMainWnd && message_ && *message_ != '\0')
  { // Successfull login - show message of the day
    CMotdDlg dlg(message_, g_szMotd, pMainWnd);
    dlg.DoModal();
  }

  // Initiate Lounge survey
  if (Global::LoungeServer())
    Global::LoungeServer()->sendLoungeSurveyPDU();
}


void PDULoungeReject::execute(Base::GameState*)
{
  if (flag_ == RF_Allow_Retry)
  {
    UINT action = AfxMessageBox(g_szInvalidNameOrPassword, MB_YESNO);
    if (action == IDYES)
    { // Allow user to retry the login
      AfxGetMainWnd()->PostMessage(WM_COMMAND,
                                   MAKEWPARAM(ID_LOGIN, 0),
                                   0);
    }
    else
    {
        Global::SetCloseConnection(true);
        if (Global::LoungeServer())
            Global::Instance().closeLoungeServer();

        CFloorMap* pFM = CFloorMap::Instance();
        if (pFM)
            pFM->disconnect();
    }
  }
  else
  { //
    // No login retry allowed - inform player and
    // terminate connection
    //
    Global::SetCloseConnection(true);
    if (Global::LoungeServer())
        Global::Instance().closeLoungeServer();

    CFloorMap* pFM = CFloorMap::Instance();
    if (pFM)
        pFM->disconnect();

    AfxMessageBox(g_szLoginFailed);
  }
}


void PDULoungeStatus::execute(Base::GameState*)
{
  CFloorMap* pFM = CFloorMap::Instance();
  if (pFM)
  {
    QueueEntry* pQueue = pQueues_;
    for (int i = 0; i < numQueues_; i++, pQueue++)
      pFM->addQueue(pQueue->queuetype_,
                    pQueue->queuenumber_,
                    pQueue->queuesize_,
                    CChips_n(pQueue->lolimit_).ntohl(),
                    CChips_n(pQueue->hilimit_).ntohl());
    pFM->recalcLayout();
    pFM->Invalidate();
  }
}


void PDULoungeTableFull::execute(Base::GameState*)
{
}


void PDULoungeSitting::execute(Base::GameState*)
{
  if (CFloorMap::Instance())
  {
    u_int16_t gameType = 0;
    CChips lo = 0, hi = 0;
    u_int16_t isHiLo = 0;
    CFloorMap::Instance()->getTableLimits(table_,
                                          lo, hi,
                                          isHiLo,
                                          gameType);

    // No questions asked because multiple dialogs
    // popping up might cause problems if the user joins
    // multiple tables at one time
    //CString msg;
    //msg.Format(g_szConfirmFmt, table_, lo, hi);
    //AfxMessageBox(msg, MB_ICONQUESTION|MB_YESNO);
    
    UINT cmd = IDYES;

    if (cmd == IDYES)
    {
      ULONG launch_id = 0;
      if (LaunchTable(Network::Server::Username(),
                      Network::Server::Password(),
                      gameType, isHiLo, table_, lo, hi, host_, port_,
                      AfxGetMainWnd()->GetSafeHwnd()))
      {
        CFloorMap::Instance()->joinedTable(table_, lo, hi, launch_id);
      }
      else
      { // Launch failed!
        if (Global::LoungeServer())
          Global::LoungeServer()->sendLoungeCancelSitDownPDU(table_);
        CFloorMap::Instance()->launchFailed(table_, lo, hi);
      }
    }
    else
    { // Send cancel sitdown!
      if (Global::LoungeServer())
        Global::LoungeServer()->sendLoungeCancelSitDownPDU(table_);
    }
  }
}

void PDULoungeObserving::execute(Base::GameState*)
{
  if (CFloorMap::Instance())
  {
    u_int16_t gameType = 0, isHiLo = 0;
    CChips lo = 0, hi = 0;    
    CFloorMap::Instance()->getTableLimits(table_,
                                          lo, hi,
                                          isHiLo,
                                          gameType);

    // No questions asked because multiple dialogs
    // popping up might cause problems if the user joins
    // multiple tables at one time

    ULONG launch_id = 0;
    LaunchTable(Network::Server::Username(),
                Network::Server::Password(),
                gameType, isHiLo, table_, lo, hi, host_, port_,
                AfxGetMainWnd()->GetSafeHwnd(),
                "1"); // observing!
  }
}


void PDULoungeLogoutWarning::execute(Base::GameState*)
{
  if (AfxMessageBox(g_szLogoutWarning, MB_YESNO) == IDYES)
  {
    if (Global::LoungeServer())
      Global::LoungeServer()->sendLoungeLogoutCertainPDU();
  }
}


void PDULoungeLogoutConfirmed::execute(Base::GameState*)
{
  if (Global::LoungeServer())
  {
    Global::Instance().closeLoungeServer();
    Network::Server::Quit();
  }

  if (CFloorMap::Instance())
  {
    CFloorMap::Instance()->disconnect();
    CFloorMap::Instance()->Invalidate();
  }
}

#endif // !PSPOT_TABLE_MODULE_

} // Network