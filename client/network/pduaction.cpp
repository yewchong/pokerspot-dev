/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUAction
  Created  : 11.11.1999

  OVERVIEW : Action PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduaction.h"
#include "base/gamestate.h"
#include "ui/cmdid.h"


BEGIN_NAMESPACE(Network)

void PDUAction::execute(GameState* pS)
{
  if (pS)
  {
    // Tell state to visualize the action
    pS->onCommand(MAKEWPARAM(ID_PDU_ACTION, 0),
                  reinterpret_cast<LPARAM>(this));
  }
}


END_NAMESPACE(Network)