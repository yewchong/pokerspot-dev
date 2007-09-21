/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUShowdown
  Created  : 14.11.1999

  OVERVIEW : Showdown PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdushowdown.h"
#include "base/player.h"
#include "base/card.h"
#include "base/gamelogic.h"
#include "base/stateshowdown.h"
#include "ui/tableview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Network
{

using Base::Player;
using Base::Card;
using Base::GameLogic;

PDUShowdown::PDUShowdown(const char* buf)
  :
  PDUHeader (buf),
  player_   (0),
  num_cards_(0),
  msglen_   (0),
  message_  (0),
  pCards_   (0)
{
  u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
  player_ = ntohs(*pint++);
  num_cards_ = ntohs(*pint++);
  msglen_ = ntohs(*pint++);

  // next come the cards
  if (num_cards_ > 0)
    pCards_ = new CardEntry[num_cards_];
  if (pCards_)
  { 
    memset(pCards_, 0, num_cards_ * sizeof(CardEntry));
    CardEntry* pC = pCards_;

    for (int i = 0; i < num_cards_; i++, pC++)
    {
      pC->suit_ = ntohs(*pint++);
      pC->value_ = ntohs(*pint++);
    }
  }

  if (msglen_ > 0)
  {
    message_ = new char[msglen_ + 1];
    if (message_)
    {
      memcpy(message_, (char*)pint, msglen_);
      message_[msglen_] = '\0';
    }
  }
}


void PDUShowdown::execute(Base::GameState*)
{
  CTableView* pView = CTableView::Instance();

  if (pView)
  {
    Player* pPlayer = pView->getPlayer(player_);
    if (pPlayer)
    {
      if (num_cards_ == 0)
      { //
        // Player mucks hand - do muck hand animation
        // unless its the local player
        //
        if (!pView->isLocalPlayer(player_))
        {
          pView->InvalidateRect(pPlayer->getCardPaintArea(Player::AllCards));
          pPlayer->empty();
          GameLogic::Instance().setNextState(
              new StateShowdown(player_, pPlayer->numCards()));
        }
      }
      else
      { //
        // Show player's cards
        //
        pView->InvalidateRect(pPlayer->getCardPaintArea(Player::AllCards));
        CardEntry* pC = pCards_;
        for (int i = 0; i < num_cards_; i++, pC++)
          pPlayer->setCard(i, Card((Base::Suit)pC->suit_, pC->value_));
        pPlayer->setShowAll(TRUE);
      }

      pView->InvalidateRect(
        pPlayer->getCardPaintArea(Player::AllCards));

      if (message_)
        pPlayer->setActionText(message_);
      else
        pPlayer->setActionText("");
    }
  }
}


}

