/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUActionEcho
  Created  : 28.12.1999

  OVERVIEW : Action Echo PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduactionecho.h"
#include "network/pduaction.h"
#include "network/server.h"
#include "base/gamestate.h"
#include "base/gamelogic.h"
#include "base/player.h"
#include "base/actionimpl.h"
#include "base/misc.h"
#include "tem/playerindex.h"
#include "tem/statebet.h"
#include "ui/tableview.h"
#include "ui/global.h"
#include "ui/chatview.h"
#include "ui/cmdid.h"


BEGIN_NAMESPACE(Network)

using Base::GameLogic;

void PDUActionEcho::execute(GameState* pS)
{
  // Call the corresponding routine to
  // visualize the action.

  CTableView* pView = CTableView::Instance();
  if (!pView) return;
  Player* pPlayer = pView->getPlayer(slot_);
  if (!pPlayer) return;

  if (pS)
  { // Let the current state know of the action
    pS->onCommand(MAKEWPARAM(ID_PDU_ACTIONECHO, 0),
                  reinterpret_cast<LPARAM>(this));
  }

  // Get the state anew because actionecho handling might
  // have destroyed it
  pS = GameLogic::Instance().currentState();

  if (action_ == PDUAction::Call)
  { 
    Base::ImplementCall(pPlayer, slot_, amount_);
  }
  else if (action_ == PDUAction::Raise)
  {     
    CChips bet;
    if (pS)
    { // Ask state the current bet
      pS->onCommand(MAKEWPARAM(ID_PDU_GETBET, 0),
                    reinterpret_cast<LPARAM>(&bet));
    }

    Base::ImplementRaise(pPlayer, slot_, bet, amount_);
  }
  else if (action_ == PDUAction::Fold)
  {
    Base::ImplementFold(pPlayer, slot_);

    if (pView->isLocalPlayer(slot_) && Base::GameLogic::LeaveTable())
    { // User has clicked the Leave Table button
      // - send logout now
      Base::SetStatusText(pView, _T("Leaving table..."));
      if (Global::TableServer()) 
        Global::TableServer()->sendSetupTableLogout();
    }
  }
  else if (action_ == PDUAction::Ante)
  {
    Base::ImplementAnte(pPlayer, slot_, amount_);
  }
  else if (action_ == PDUAction::SitOut)
  {
    Base::ImplementSitOut(slot_, pPlayer);

    if (pView->isLocalPlayer(slot_) && Base::GameLogic::LeaveTable())
    { // User has clicked the Leave Table button
      // - send logout now
      Base::SetStatusText(pView, _T("Leaving table..."));
      if (Global::TableServer()) 
        Global::TableServer()->sendSetupTableLogout();
    }
  }
  else
  {
    CString s;
    s.Format("*** Action Echo PDU: unrecognized action: %d", action_);
    PDU_DEBUG(s);
  }
}


END_NAMESPACE(Network)