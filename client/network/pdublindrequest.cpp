/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUBlindRequest
  Created  : 12.11.1999

  OVERVIEW : Blind Request PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdublindrequest.h"
#include "network/server.h"
#include "base/gamelogic.h"
#include "base/player.h"
#include "tem/statepromptpostblind.h"
#include "ui/tableview.h"
#include "ui/global.h"
#include "ui/cmdid.h"
#include "common/stringoutput.h"

BEGIN_NAMESPACE(Network)


using Base::GameLogic;
using Base::GameState;

void PDUBlindRequest::execute(GameState* pState)
{
  CTableView* pView = CTableView::Instance();
  if (!pView) return;

  if (Global::TableServer())
    Global::TableServer()->setSequenceNumber(sequencenumber_);

  BOOL isSevenStud = (Global::GetGameType() == GT_SevenStud);
  if (isSevenStud)
  { // In 7-stud everybody antes, we should reply to this
    player_ = pView->getLocalPlayerIndex();

    // Change all player's prompts to To Ante
    for (int i = 0; i < CTableView::MaxPlayers; i++)
    {
      Player* p = pView->getPlayer(i);
      if (p && !p->isSittingOut())
      {
        CStrOut s;
        s << "To Ante " << value_;
        // XXX OLD CString s;
        //s.Format("To Ante %d", value_);
        p->setActionText(s.str());
      }
    }
  }

  bool autoReplied = false;

//  if (!autoReplied)
  {
  //
  // Set up the Blind Post Prompt state even if
  // we replied already because the state will
  // execute the animation on receiving the action
  // echo.
  //

  // Check if we already have a blind post prompt state
  GameState* state = GameLogic::Instance().currentState();
  if (state)
  {
    BOOL isPromptBlindState =
      state->onCommand(MAKEWPARAM(ID_ISPROMPTBLINDPOST, 0),
                         0);
    if (!isPromptBlindState)
      state = NULL;
  }
  else
  {
    state = NULL;
  }

  StatePromptPostBlind* pS = NULL;

  if (!state)
  { // First blind request, create state
    pS = new StatePromptPostBlind(player_, value_, msecs_);
    GameLogic::Instance().setNextState(pS);
    if (pS)
      pS->start();
  }
  else
  { // Another blind request, restart prompt
    pS = static_cast<StatePromptPostBlind*>(state);
    pS->startPrompt(player_, value_, msecs_);
  }
  if (pS && autoReplied)
    pS->endPrompt();

  }
}


END_NAMESPACE(Network)
