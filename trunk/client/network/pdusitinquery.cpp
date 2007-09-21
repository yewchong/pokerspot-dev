/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUSitInQuery
  Created  : 12.11.1999

  OVERVIEW : Sit In Query PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdusitinquery.h"
#include "base/gamelogic.h"

namespace Network
{

using Base::GameLogic;

void PDUSitInQuery::execute(Base::GameState* pState)
{
  if (GameLogic::SitOut() || GameLogic::LeaveTable())
    GameLogic::SendSitOutPDU();
  else 
    GameLogic::SendSitInPDU();
}


} // Network
