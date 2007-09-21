/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUButton
  Created  : 01/05/2000

  OVERVIEW : Button PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdubutton.h"
#include "base/player.h"
#include "ui/tableview.h"
#include "base/gamelogic.h"
#include "tem/statepromptpostblind.h"


BEGIN_NAMESPACE(Network)

using Base::GameLogic;

//
// This PDU is always sent at the start of hand.
//
// The dealer is set twice to make sure the old
// blind positions are erased and new positions
// painted correctly.
//
// The player states are reset to sit-in.
//
void PDUButton::execute(Base::GameState* pState)
{
  CTableView* pView = CTableView::Instance();
  if (pView)
  { 
    pView->resetCards(true);
    pView->setDealer(dealerslot_, true);

    static bool is_first_button = true;
    if (!is_first_button)
    {
      for (int i = 0; i < 10; i++)
      {
        Player* pP = pView->getPlayer(i);
        if (pP)
        {
          if (pP->getChips() > 0 &&
              pP->getState() != Base::AllIn)
          {
            pP->setState(Base::SitIn);
            pP->setActionText("", true);
          }
          else
          {
            pP->setState(Base::SitOut);
          }
        }
      }
    }
    else
      is_first_button = true;

    pView->setDealer(dealerslot_, true);
    pView->Invalidate();
  }

  if (GameLogic::Instance().currentState() == NULL)
  { // Set the next state already!
    GameLogic::Instance().setNextState(new StatePromptPostBlind(10, 0, 0));
  }
}

END_NAMESPACE(Network)
