/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUActionRequestSpreadLimit
  Created  : 07/02/2000

  OVERVIEW : Action Request PDU allowing spread limit raises.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduactionrequestspreadlimit.h"
#include "network/server.h"
#include "base/gamestate.h"
#include "base/gamelogic.h"
#include "tem/statebet.h"
#include "ui/cmdid.h"
#include "ui/global.h"


BEGIN_NAMESPACE(Network)

USING_NS_T(Base, GameLogic);

void PDUActionRequestSpreadLimit::execute(GameState* pS)
{
#ifdef PSPOT_TABLE_MODULE_
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
    pS->onCommand(MAKEWPARAM(ID_PDU_ACTIONREQUESTSPREADLIMIT, 0),
                  reinterpret_cast<LPARAM>(this));
  }
#endif
}


END_NAMESPACE(Network)

