/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class StateDealPocket2
  Created  : 04/11/2000

  OVERVIEW : New Deal Pocket cards state. Can deal any number
             of pocket cards.

 ****************************************************************************/

#include "stdafx.h"
#include "base/statedealpocket2.h"
#include "base/player.h"
#include "base/gamelogic.h"
#include "base/card.h"
#include "base/cards.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/soundmanager.h"
#include "ui/cmdid.h"
#include "ui/global.h"
#include "tem/statebet.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif


using Base::GameLogic;
using Base::Player;
using Base::Card;

// Move card from 'deck' to player.
Card MoveCardToPlayer(std::list<Card>& deck, Player* pPlayer)
{
  Card card;

  if (deck.size() > 0)
  { // Take the first card from deck
    std::list<Card>::iterator it = deck.begin();
    card = (*it);
    deck.erase(it);

    // and add it to player
    pPlayer->addCard(card);
  }

  return card;
}


StateDealPocket2::StateDealPocket2(
    int dealer,
    int numCards,
    Network::CardEntry* pCard,
    int msecs)
  :
  Base::AnimateState(CTableView::Instance()),
  dealerNum_        (dealer),
  currentNum_       (0),
  firstPlayer_      (0),
  pTableView_       (CTableView::Instance()),
  msecs_            (msecs),
  numCards_         (numCards),
  dummy_            (false)
#ifdef _DEBUG
  , numIters_(0)
#endif
{
  ASSERT_VALID(pTableView_);

  for (int i = 0; i < numCards_; i++, pCard++)
    cards_.push_back(Base::Card(pCard->suit_, pCard->value_));

  // Player left of dealer gets the first card
  currentNum_ = pTableView_->getNextPlayerInGame(dealerNum_);
  firstPlayer_ = currentNum_;
}


StateDealPocket2::StateDealPocket2(
    int dealer,
    int numCards,
    int msecs)
  :
  Base::AnimateState(CTableView::Instance()),
  dealerNum_        (dealer),
  currentNum_       (0),
  firstPlayer_      (0),
  pTableView_       (CTableView::Instance()),
  msecs_            (msecs),
  numCards_         (numCards),
  dummy_            (true)
#ifdef _DEBUG
  , numIters_(0)
#endif
{
  ASSERT_VALID(pTableView_);

  // Player left of dealer gets the first card
  currentNum_ = pTableView_->getNextPlayerInGame(dealerNum_);
  firstPlayer_ = currentNum_;
}


StateDealPocket2::~StateDealPocket2()
{
//  ASSERT(cards_.size() == 0);
}


BOOL StateDealPocket2::tick(DWORD dt)
{
  bool playSound = false;
  BOOL rc = TRUE;

  int maxCards = 2;
  if (Global::GetGameType() == GT_Holdem)
    maxCards = 2;
  else if (Global::GetGameType() == GT_Omaha)
    maxCards = 4;
  else if (Global::GetGameType() == GT_SevenStud)
    maxCards = 7;

  Player* pHouse = pTableView_->getHouse();
  ASSERT(pHouse);
  Player* pPlayer = pTableView_->getPlayer(currentNum_);

  if (ticks_ == 0)
  { //
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

    if (pPlayer && pPlayer->isInGame())
    {
      if (pTableView_->isLocalPlayer(currentNum_))
      { //
        // Local player's pocket cards are decided by the server
        //
        if (cards_.size() > 0)
          MoveCardToPlayer(cards_, pPlayer);
      }
      else
      { // 
        // Remote player's cards don't matter since they are
        // not shown until showdown - add dummy card
        //
        //ASSERT(pPlayer->numCards() < maxCards);
        if (pPlayer->numCards() < maxCards)
        {
          Base::Card card;
          pPlayer->addCard(card);
        }
      }

      playSound = true;

      if (msecs_ > 0)
        pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));
    }

    // Prepare to deal to next player
    currentNum_ = pTableView_->getNextPlayerInGame(currentNum_);

    if (currentNum_ == firstPlayer_)
      numCards_--;

    if (numCards_ <= 0)
    { //
      // We have dealt all pocket cards - start betting round
      //
      // If no animation, repaint all at once
      if (msecs_ == 0)
        pView_->Invalidate();

      // Destroys 'this'
      GameLogic::Instance().setNextState(new StateBet());
      return TRUE; 
    }
  }

  if (playSound)
    SndMan::Instance()->playSound(SndMan::SM_Card);

  return rc;
}


BOOL StateDealPocket2::draw(CDC* pDC)
{
  return TRUE;
}


BOOL StateDealPocket2::onCommand(WPARAM wParam, LPARAM lParam)
{
  // It is possible that the server sends the next Action
  // Request PDU before this state has finished its job.
  // We handle that case here by finishing up immediately
  // and setting the state to StateBet
  if (LOWORD(wParam) == ID_PDU_ACTIONREQUEST ||
      LOWORD(wParam) == ID_PDU_ACTIONREQUESTSPREADLIMIT)

  {
    // Finish up the dealing immediately
    finishDealCards();

    // Set the new state immediately;
    // NOTE: destroys this
    StateBet* pS = new StateBet();
    GameLogic::Instance().setNextState(pS);
    pS->onCommand(wParam, lParam);
    return TRUE;
  }

  if (LOWORD(wParam) == ID_PDU_DEALUPCARD ||
      LOWORD(wParam) == ID_FINISH_DEAL)
  { // Next deal cards pdu has arrived - finish
    finishDealCards();
    return TRUE;
  }

  return FALSE;
}


void StateDealPocket2::finishDealCards()
{
  int maxCards = 2;
  if (Global::GetGameType() == GT_Holdem)
    maxCards = 2;
  else if (Global::GetGameType() == GT_Omaha)
    maxCards = 4;
  else if (Global::GetGameType() == GT_SevenStud)
    maxCards = 7;

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
      CRect r(pos_, CSize(Base::Cards::Width_,
                          Base::Cards::Height_));
      pView_->InvalidateRect(&r);
    }
  }

  int loopGuard = 50;

  // Finish what we were doing - deal cards.
  while (pPlayer && numCards_ > 0 && --loopGuard > 0)
  {
    if (pPlayer->isInGame())
    {
      if (pTableView_->isLocalPlayer(currentNum_))
      { 
        if (cards_.size() > 0)
          MoveCardToPlayer(cards_, pPlayer);
      }
      else
      { 
//        ASSERT(pPlayer->numCards() < maxCards);
        if (pPlayer->numCards() < maxCards)
        {
          Base::Card card;
          pPlayer->addCard(card);
        }
      }

      pTableView_->InvalidateRect(&pPlayer->getCardPaintArea(Player::LastCard));
    } 

    // Prepare to deal to next player
    currentNum_ = pTableView_->getNextPlayerInGame(currentNum_);
    pPlayer = pTableView_->getPlayer(currentNum_);

    if (currentNum_ == firstPlayer_)
      numCards_--;
  }
}