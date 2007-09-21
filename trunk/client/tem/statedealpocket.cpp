/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateDealPocket
  Created  : 10.11.1999

  OVERVIEW : Implements the dealing of pocket cards for Texas Hold'em.

 ****************************************************************************/

#include "stdafx.h"
#include "tem/statedealpocket.h"
#include "tem/statebet.h"
#include "tem/playerindex.h"
#include "base/player.h"
#include "base/cards.h"
#include "base/gamelogic.h"
#include "ui/tableview.h"
#include "ui/cmdid.h"
#include "ui/soundmanager.h"

using Base::Cards;
using Base::Card;
using Base::GameLogic;

//
// STATE: StateDealPocket
//        Deal initial face-down pocket cards to players.
//
// Entry: Deal Cards PDU has arrived
//
// PDUs:  None - if an Action Request PDU is received,
//        it means this state spent too long a time
//        in the deal sequence. In that case, deal all
//        remaining cards immediately and exit.
//
// Exit:  StateBet on pocket cards have been dealt.
//

StateDealPocket::StateDealPocket(int dealer,
                                 int card1, int suit1,
                                 int card2, int suit2,
                                 int msecs)
  :
  Base::AnimateState(CTableView::Instance()),
  dealerNum_        (dealer),
  currentNum_       (0),
  pTableView_       (CTableView::Instance()),
  card1_            (card1),
  suit1_            (suit1),
  card2_            (card2),
  suit2_            (suit2),
  msecs_            (msecs)
{
  ASSERT_VALID(pTableView_);

  // Player left of dealer gets the first card
  currentNum_ = pTableView_->getNextPlayerInGame(dealerNum_);
}


StateDealPocket::~StateDealPocket()
{}


BOOL StateDealPocket::tick(DWORD dt)
{
  bool playSound = false;
  BOOL rc = TRUE;

  ASSERT_VALID(pTableView_);

  if (ticks_ == 0)
  { //
    // Start dealing new card.
    //
    Player* pHouse = pTableView_->getHouse();
    ASSERT(pHouse);
    Player* pPlayer = pTableView_->getPlayer(currentNum_);

    if (pPlayer) // left in the middle of deal?
    { 
      if (pPlayer->numCards() == NumFacedownCards)
      { //
        // We have dealt all pocket cards - first betting round starts
        //
        // If no animation, repaint all at once
        if (msecs_ == 0)
          pView_->Invalidate();

        // Destroys 'this'
        GameLogic::Instance().setNextState(new StateBet());
        return TRUE; 
      }

      // Get start and end positions for card animation
      pos_ = start_ = pHouse->getCardPos();
      end_ = pPlayer->getCardPos(pPlayer->numCards());

      if (msecs_ > 0)
        startAnimate();
    }
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
    // now actually move the card from house to player
    //
    ticks_ = 0;

    if (msecs_ > 0)
      stopAnimate();

    Player* pHouse = pTableView_->getHouse();
    ASSERT(pHouse);
    Player* pPlayer = pTableView_->getPlayer(currentNum_);

    if (pPlayer) // left in the middle of deal?
    {
      Card card(0, 0);

      bool is_local = pTableView_->isLocalPlayer(currentNum_);
      if (is_local)
      { //
        // Local player's pocket cards are decided by the server
        //
        if (pPlayer->numCards() == 0)
          card = Card(suit1_, card1_);
        else
          card = Card(suit2_, card2_);
      }
      else
      { // 
        // Remote player's cards don't matter since they are
        // not shown until showdown
        //
      }

      // Add the card to player's deck
      if ((card1_ != 0 && card2_ != 0) || !is_local)
      { // card value 0 denotes a dummy card for
        // a sit-out player
        pos_ = CPoint(-1, -1);
        if (pPlayer->isInGame())
        {
          pPlayer->addCard(card);
          playSound = true;
        }
      }

      if (msecs_ > 0)
        pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));
    }

    // Prepare to deal to next player
    currentNum_ = pTableView_->getNextPlayerInGame(currentNum_);
  }

  if (playSound)
    SndMan::Instance()->playSound(SndMan::SM_Card);

  return rc;
}


BOOL StateDealPocket::draw(CDC* pDC)
{
  if (ticks_ > 0 && ticks_ < msecs_)
  {
    Cards::Instance().drawCard(pDC, Card(Base::Hearts, 1),
                               pos_.x, pos_.y, Cards::C_BackSide);
  }
  return TRUE;
}


BOOL StateDealPocket::onCommand(WPARAM wParam, LPARAM lParam)
{
  // It is possible that the server sends the next Action
  // Request PDU before this state has finished its job.
  // We handle that case here by finishing up immediately
  // and setting the state to StateBet
  if (LOWORD(wParam) == ID_PDU_ACTIONREQUEST ||
      LOWORD(wParam) == ID_PDU_ACTIONREQUESTSPREADLIMIT)
  {
    // Finish up the dealing immediately
    Player* pHouse = pTableView_->getHouse();
    ASSERT(pHouse);
    Player* pPlayer = pTableView_->getPlayer(currentNum_);
    if (!pPlayer)
    {
      pView_->Invalidate(); // left in the middle of deal
    }
    else
    { // Clean up the animation if any
      if (pos_ != CPoint(-1, -1))
      {
        CRect r(pos_, CSize(Cards::Width_, Cards::Height_));
        pView_->InvalidateRect(&r);
      }
    }


    while (pPlayer && pPlayer->numCards() != NumFacedownCards)
    {
      Card card(Base::Hearts, 0);
      if (pTableView_->isLocalPlayer(currentNum_))
      { //
        // Local player's pocket cards are decided by the server
        //
        if (pPlayer->numCards() == 0)
        {
          card.suit((Base::Suit)suit1_);
          card.number(card1_);
        }
        else
        {
          card.suit((Base::Suit)suit2_);
          card.number(card2_);
        }
      }
      else
      { // 
        // Remote player's cards don't matter since they are
        // not shown until showdown
        //
      }

      if (pPlayer->isInGame())
      {
        // Add the card to player's deck
        pPlayer->addCard(card);
        pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));
      }
  
      // Prepare to deal to next player
      currentNum_ = pTableView_->getNextPlayerInGame(currentNum_);
      pPlayer = pTableView_->getPlayer(currentNum_);
    }

    // Set the new state immediately;
    // NOTE: GameLogic::setNextState destroys this!
    StateBet* pS = new StateBet();
    GameLogic::Instance().setNextState(pS);
    pS->onCommand(wParam, lParam);
    return TRUE;
  }
  return FALSE;
}



