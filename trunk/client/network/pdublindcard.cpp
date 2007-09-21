/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUBlindCard
  Created  : 12.12.1999

  OVERVIEW : Blind Card PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdublindcard.h"
#include "base/player.h"
#include "base/card.h"
#include "ui/tableview.h"

BEGIN_NAMESPACE(Network)


using Base::Player;
using Base::Card;

void PDUBlindCard::execute(Base::GameState*)
{
  CTableView* pView = CTableView::Instance();
  ASSERT(pView);

  if (pView)
  {
    Player* pPlayer = pView->getPlayer(player_);
    if (pPlayer)
    { // add blind card
      pPlayer->addCard(Card((Base::Suit)1, 0));
      pView->InvalidateRect(pPlayer->getCardPaintArea(Player::LastCard));
    }
    else
    {
      CString s;
      s.Format("*** Blind Card PDU error: player %d not found", player_);
      PDU_DEBUG(s);
    }
  }  
}



void PDUBlindCard2::execute(Base::GameState*)
{
  CTableView* pView = CTableView::Instance();
  ASSERT(pView);

  if (pView)
  {
    Player* pPlayer = pView->getPlayer(player_);
    if (pPlayer)
    { // add blind card
      pPlayer->addCard(Card((Base::Suit)1, 0));
      pView->InvalidateRect(pPlayer->getCardPaintArea(Player::LastCard));
    }
    else
    {
      CString s;
      s.Format("*** Blind Card PDU error: player %s not found", player_);
      PDU_DEBUG(s);
    }
  }  
}

END_NAMESPACE(Network)
