/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUDealCards
  Created  : 10.11.1999

  OVERVIEW : Deal Cards PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdudealcards.h"
#include "base/gamelogic.h"
#include "base/player.h"
#include "tem/statedealpocket.h"
#include "ui/tableview.h"


BEGIN_NAMESPACE(Network)

void ClearCardsFromTable(CTableView* pTable)
{
  // This is basically not necessary because the
  // button PDU in the start of every hand clears
  // the table. However, we make it sure here anyway
  // because of the fake PDUs that are used for debugging.
  if (pTable->getCommunity()->numCards() != 0 ||
      (pTable->getPlayer(0) && pTable->getPlayer(0)->numCards() != 0))
  { 
    pTable->resetCards(true); 
  }
}


USING_NS_T(Base, GameLogic);
USING_NS_T(Base, GameState);

void PDUDealCards::execute(GameState*)
{
  CTableView* pTable = CTableView::Instance();
  if (pTable)
  {
    ClearCardsFromTable(pTable);

    /* NOTE: the dealer position is now specified with a Button PDU
             and saved to the view */
    GameLogic::Instance().setNextState(new StateDealPocket(pTable->dealer(),
                                                           card1_,
                                                           suit1_,
                                                           card2_,
                                                           suit2_,
                                                           msecs_));
  }
}


END_NAMESPACE(Network)