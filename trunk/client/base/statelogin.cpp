/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateLogin
  Created  : 29.10.1999

  OVERVIEW : Implements the log in state that waits
             until new hand starts.

 ****************************************************************************/

#include "stdafx.h"
#include "base/statelogin.h"
#include "base/gamelogic.h"
#include "network/network.h"
#include "ui/mainwnd.h"
#include "ui/resource.h"
#include "ui/waitdlg.h"


namespace Base
{

using namespace Network;

//
// STATE: Log in.
//
StateLogin::StateLogin(const CString& username,
                       const CString& password)
  :
  pWaitDlg_(0),
  retry_   (2),
  ticks_   (0)

{
  CMainWnd* pWnd = CMainWnd::Instance();
  ASSERT(pWnd);
/*
  pWaitDlg_ = new CWaitDlg(pWnd);
  pWaitDlg_->Create(IDD_LOGGINGIN, pWnd);
  pWaitDlg_->ShowWindow(SW_SHOW);
*/
  // XXX Disable main window!
  pWnd->EnableWindow(FALSE);

  // DPP::SendLoginPDU(username_, password_);
}


StateLogin::~StateLogin()
{
/*
  if (pWaitDlg_)
  {
    pWaitDlg_->DestroyWindow();
    // XXX DOUBLE DELETE!!!
    delete pWaitDlg_;
    pWaitDlg_ = 0;
  }
*/
  CMainWnd* pWnd = CMainWnd::Instance();
  ASSERT(pWnd);
  pWnd->EnableWindow(TRUE);
}


BOOL StateLogin::tick(DWORD dt)
{
  // Could update the prompt to show login progress
  if (pWaitDlg_)
  { // Simulate logging states
    ASSERT_VALID(pWaitDlg_);
    if (ticks_  < 5000)
    {
      static BOOL s = 0;
      if (!s)
        s = TRUE, pWaitDlg_->setPrompt("Contacting host...");
    }
    else if (ticks_ < 15000)
    {
      static BOOL s = 0;
      if (!s)
        s = TRUE, pWaitDlg_->setPrompt("Logging in...");
    }
    else
    {
      static BOOL s = 0;
      if (!s)
        s = TRUE, pWaitDlg_->setPrompt("Login ok...");
    }     
  }
  
  ticks_ += dt;
  if (ticks_ > 20000)
  {
    // Destroys 'this'
    GameLogic::Instance().setNextState(0);
  }
  return TRUE;
}


BOOL StateLogin::draw(CDC* pDC)
{
  return TRUE;
}


BOOL StateLogin::onCommand(WPARAM wParam, LPARAM lParam)
{
  BOOL rc = FALSE;

  if (!rc)
    rc = GameState::onCommand(wParam, lParam);

  return rc;
}

} // Base