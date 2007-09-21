/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class StateDealUpCard
  Created  : 06/22/2000

  OVERVIEW : Deal up cards to specified player.

 ****************************************************************************/


#include "stdafx.h"
#include "base/statedealupcard.h"
#include "base/player.h"
#include "base/cards.h"
#include "base/gamelogic.h"
#include "tem/statebet.h"
#include "ui/tableview.h"
#include "ui/cmdid.h"
#include "ui/soundmanager.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

using Base::GameLogic;
using Base::Player;
using Base::Card;

// Move card from 'deck' to player.
extern Card MoveCardToPlayer(std::list<Card>& deck, Player* pPlayer);

StateDealUpCard::StateDealUpCard(
    int player,                  
    int numCards,
    Network::CardEntry* pCard,
    int msecs)
  :
  Base::AnimateState(CTableView::Instance()),
  player_           (player),
  pTableView_       (CTableView::Instance()),
  msecs_            (msecs)
{
  ASSERT_VALID(pTableView_);

  for (int i = 0; i < numCards; i++, pCard++)
    cards_.push_back(Base::Card(pCard->suit_, pCard->value_));

  // When new upcards are dealt the previous
  // betting round has ended.
  pTableView_->endBettingRound();
  pTableView_->UpdateWindow();
}


StateDealUpCard::~StateDealUpCard()
{}


void StateDealUpCard::dealUpCard(int player,
                                 int numCards,
                                 Network::CardEntry* pCard,
                                 int msecs)
{
  finishCurrentDeal();

  ASSERT(cards_.size() == 0);
  cards_.clear();

  // start new deal
  player_ = player;
  ticks_ = 0;
  msecs_ = msecs;

  for (int i = 0; i < numCards; i++, pCard++)
    cards_.push_back(Base::Card(pCard->suit_, pCard->value_));
}

BOOL StateDealUpCard::tick(DWORD dt)
{
  bool playSound = false;
  BOOL rc = TRUE;

  Player* pHouse = pTableView_->getHouse();
  ASSERT(pHouse);
  Player* pPlayer = pTableView_->getPlayer(player_);

  if (ticks_ == 0)
  {
    if (cards_.size() == 0)
      return FALSE;

    //
    // Start dealing new card.
    //
    if (pPlayer) // left in the middle of deal?
    { 
      // Get start and end positions for card animation
      pos_ = start_ = pHouse->getCardPos();
      end_ = pPlayer->getCardPos(pPlayer->numCards());

      if (msecs_ > 0)
        startAnimate();
    }
    ticks_ += 1;
  }

  ticks_ += dt;

  if ((ticks_ < msecs_) && (msecs_ > 0))
  { //
    // Do a piece of the dealing animation
    //
    CPoint diff = end_ - start_;
    float d = float(ticks_) / float(msecs_);
    diff.x *= d;
    diff.y *= d;
    CPoint newPos = start_ + diff;

    animate(newPos, pos_);
    pos_ = newPos;
  }
  else 
  { // 
    // Dealing animation for current card finished -
    // now actually move the card from deck to player
    //
    ticks_ = 0;
    if (msecs_ > 0)
      stopAnimate();
    pos_ = CPoint(-1, -1);

    if (pPlayer)     // might leave in the middle of deal?
    {
      if (cards_.size() > 0)
      {
        MoveCardToPlayer(cards_, pPlayer);
      }

      playSound = true;

      if (msecs_ > 0)
        pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));

      if (cards_.size() == 0)
      { //
        // We have dealt all pocket cards - finish
        //
      }
    }
    else
    {
      cards_.clear();
    }
  }

  if (playSound)
    SndMan::Instance()->playSound(SndMan::SM_Card);

  return rc;
}


BOOL StateDealUpCard::draw(CDC* pDC)
{
  return TRUE;
}


BOOL StateDealUpCard::onCommand(WPARAM wParam, LPARAM lParam)
{
  if (LOWORD(wParam) == ID_ISDEALUPCARD)
    return TRUE;

  // It is possible that the server sends the next Action
  // Request PDU before this state has finished its job.
  // We handle that case here by finishing up immediately
  // and setting the state to StateBet
  if (LOWORD(wParam) == ID_PDU_ACTIONREQUEST ||
      LOWORD(wParam) == ID_PDU_ACTIONREQUESTSPREADLIMIT ||
      LOWORD(wParam) == ID_PDU_ACTIONECHO)
  {
    finishCurrentDeal();

    // Set the new state immediately;
    // NOTE: destroys this
    StateBet* pS = new StateBet();
    GameLogic::Instance().setNextState(pS);
    pS->onCommand(wParam, lParam);
    return TRUE;
  }
  return FALSE;
}

void StateDealUpCard::finishCurrentDeal()
{
  // Finish up the dealing immediately
  Player* pHouse = pTableView_->getHouse();
  ASSERT(pHouse);
  Player* pPlayer = pTableView_->getPlayer(player_);
  if (!pPlayer)
  {
    pView_->Invalidate(); // left in the middle of deal
  }
  else
  { // Clean up the animation if any
    if (pos_ != CPoint(-1, -1))
    {
      CRect r(pos_, CSize(Base::Cards::Width_,
                          Base::Cards::Height_));
      pView_->InvalidateRect(&r);
    }
  }

  // Finish what we were doing - deal cards.
  while (pPlayer && cards_.size() > 0)
  {
    MoveCardToPlayer(cards_, pPlayer);
    pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));
  }

  stopAnimate(); 
  msecs_ = 0;
  ticks_ = 0;
}