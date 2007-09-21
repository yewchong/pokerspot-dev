/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUShowCards
  Created  : 01/20/2000

  OVERVIEW : Show Cards Query PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdushowcardsquery.h"
#include "base/gamelogic.h"

BEGIN_NAMESPACE(Network)

using Base::GameLogic;

void PDUShowCardsQuery::execute(GameState* pState)
{
  if (GameLogic::HideHand())
    GameLogic::SendHideCardsPDU();
  else 
    GameLogic::SendShowCardsPDU();
}


END_NAMESPACE(Network)
