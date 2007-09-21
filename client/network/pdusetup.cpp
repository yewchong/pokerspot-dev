/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : pdusetup.h
  Created  : 11.12.1999

  OVERVIEW : Setup PDUs.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdusetup.h"
#include "network/server.h"
#include "base/gamelogic.h"
#include "base/misc.h"
#include "base/statewait.h"
#include "ui/buyinquerydlg.h"
#include "ui/global.h"
#include "ui/tableview.h"
#include "ui/cmdid.h"

namespace
{
  LPCTSTR g_szLoginRejected = _T("The Table Server rejected the log in.");
}

namespace Network
{

using Base::GameLogic;

//#pragma message ("*** Automatic buy-in. This must not be left here *** \n")
//#define TEST_LOGINLOGOUT_

void PDUSetupBuyInQuery::execute(Base::GameState*)
{
#ifdef TEST_LOGINLOGOUT_

    Global::TableServer()->sendSetupBuyInRequestPDU(limit_);

#else

  CBuyInQueryDlg dlg(limit_, AfxGetMainWnd());
  if (dlg.DoModal() == IDOK)
  {
    if (Global::TableServer())
      Global::TableServer()->sendSetupBuyInRequestPDU(dlg.chips_);
  }

#endif
}


void PDUSetupTableAccept::execute(Base::GameState*)
{
  Network::Server::Username(MakeString(username_));
  Network::Server::Password(MakeString(password_));

  // Initially Sitting-Out until user clicks Sit In
  GameLogic::SitOut(TRUE);

#ifndef PSPOT_TABLE_MODULE_
  // Table does not display motd
  CWnd* pMainWnd = AfxGetMainWnd();
  if (pMainWnd)
  { // Successfull login - show message of the day
    CMotdDlg dlg(message_, g_szMotd, pMainWnd);
    dlg.DoModal();
  }
#endif

  if (CTableView::Instance())
    CTableView::Instance()->recreateLeaveTableButton(ID_LOGOUT);

  // Wait for hand start
  GameLogic::Instance().setNextState(new Base::StateWait());
}


void PDUSetupTableReject::execute(Base::GameState*)
{
  if (AfxGetMainWnd())
    AfxGetMainWnd()->MessageBox(g_szLoginRejected);

  if (CTableView* pView = CTableView::Instance())
  {
    for (int i = 0; i < 10; i++)
      pView->removePlayer(i);
    pView->Invalidate();
  }

  Network::Server::Quit();
  if (Global::TableServer())
    Global::Instance().closeTableServer();
}


void PDUSetupTableLogoutConfirm::execute(Base::GameState*)
{
  if (CTableView* pView = CTableView::Instance())
  {
    pView->resetCards();
    pView->announce("");
    pView->setDealer(-1, false);

    for (int i = 0; i < 10; i++)
      pView->removePlayer(i); 
    pView->Invalidate();

    Base::SetStatusText(pView, _T(""));
  }
    
  Network::Server::Quit();
  if (Global::TableServer())
    Global::Instance().closeTableServer();
}

}
