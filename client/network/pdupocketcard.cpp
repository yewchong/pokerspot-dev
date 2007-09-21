/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPocketCard
  Created  : 12.12.1999

  OVERVIEW : Pocket Card PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdupocketcard.h"
#include "base/player.h"
#include "base/card.h"
#include "ui/tableview.h"


namespace Network
{

using Base::Player;
using Base::Card;


void PDUPocketCard::execute(Base::GameState*)
{
  CTableView* pView = CTableView::Instance();
  ASSERT(pView);

  if (pView)
  {
    Player* pPlayer = pView->getLocalPlayer();
    if (pPlayer)
    { // Add card directly to player's deck
      pPlayer->addCard(Card((Base::Suit)suit_, card_));
      pView->InvalidateRect(pPlayer->getCardPaintArea(Player::LastCard));
    }
  }  
}

}
