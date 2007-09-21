/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StatePromptStartPlay
  Created  : 28.12.1999

  OVERVIEW : Prompts the user to start playing upon having logged
             in to a table.

 ****************************************************************************/

#include "stdafx.h"
#include "base/statepromptstartplay.h"
#include "base/gamelogic.h"
#include "ui/tableview.h"
#include "ui/mainwnd.h"
#include "ui/actionbar.h"
#include "ui/cmdid.h"

namespace
{
  LPCTSTR g_szStart = _T("Sit In");
  LPCTSTR g_szStartPrompt = _T("Click 'Sit In' to join the game");
}

using namespace Network;
using Base::GameLogic;


//
// STATE: StatePromptStartPlay
//
// Entry: Successfull table login.
//
// Exit:  User clicks "Join"
//

StatePromptStartPlay::StatePromptStartPlay()
{
  if (CActionBar::Instance())
  {
    CActionBar::Instance()->setButtonText(0, g_szStart);
    CActionBar::Instance()->enableButtons(1);
    CActionBar::Instance()->enableCheckBoxes(FALSE);
  }
  CMainWnd::SetStatusText(g_szStartPrompt);
}


StatePromptStartPlay::~StatePromptStartPlay()
{
  if (CActionBar::Instance())
  {
    CActionBar::Instance()->enableCheckBoxes(TRUE);
    CActionBar::Instance()->enableButtons(0);
  }
  CMainWnd::SetStatusText("");
}


BOOL StatePromptStartPlay::onTimer(UINT msg, UINT eventId, DWORD dwTime)
{
  if (CActionBar::Instance())
    CActionBar::Instance()->animate(500);
  return TRUE;
}


BOOL StatePromptStartPlay::onCommand(WPARAM wParam, LPARAM lParam)
{
  int cmdId = LOWORD(wParam);

  BOOL rc = FALSE;

  switch (cmdId)
  { //
    // ID_BTN_1 is Join
    //
  case ID_BTN_1:
    GameLogic::SitOut(FALSE);
    GameLogic::Instance().setNextState(0); // deletes 'this'
    rc = TRUE;
    break;
  }
    
  if (!rc)
    rc = GameState::onCommand(wParam, lParam);

  return rc;
}


BOOL StatePromptStartPlay::onKey(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  return FALSE;
}


BOOL StatePromptStartPlay::tick(DWORD)
{ // no idle time processing
  return FALSE;
}


BOOL StatePromptStartPlay::draw(CDC*)
{
  return FALSE;
}
