/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUActionRequest
  Created  : 11.11.1999

  OVERVIEW : Action Request PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduactionrequest.h"
#include "network/server.h"
#include "base/gamestate.h"
#include "base/gamelogic.h"
#include "tem/statebet.h"
#include "ui/cmdid.h"
#include "ui/global.h"


BEGIN_NAMESPACE(Network)

USING_NS_T(Base, GameLogic);

void PDUActionRequest::execute(GameState* pS)
{
  if (Global::TableServer())
    Global::TableServer()->setSequenceNumber(sequencenumber_);

  if (!pS)
  { // Set betting state if its not already
    pS = new StateBet();
    GameLogic::Instance().setNextState(pS);
  }

  if (!pS)
  {
    AfxMessageBox("Out of memory!");
  }
  else
  {
    // Tell the state to start prompting for an action
    pS->onCommand(MAKEWPARAM(ID_PDU_ACTIONREQUEST, 0),
                  reinterpret_cast<LPARAM>(this));
  }
}


END_NAMESPACE(Network)

