/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateDealCommunity
  Created  : 12.11.1999

  OVERVIEW : Implements the dealing of community cards for Texas Hold'em.

 ****************************************************************************/

#include "stdafx.h"
#include "base/statedealcommunity.h"
#include "tem/statebet.h"
#include "tem/playerindex.h"
#include "base/player.h"
#include "base/cards.h"
#include "base/suit.h"
#include "base/gamelogic.h"
#include "ui/tableview.h"
#include "ui/cmdid.h"
#include <mmsystem.h>

using Base::Cards;
using Base::Card;
using Base::GameLogic;

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif

// in statedealpocket2.cpp
extern Card MoveCardToPlayer(std::list<Card>& deck, Player* pPlayer);
// in pducommunitycard
extern void ShowCommunityCardMessage(Player* pCommunity);

StateDealCommunity::StateDealCommunity(
                      int numCards,
                      int card1, int suit1,
                      int card2, int suit2,
                      int card3, int suit3,
                      int msecs)
  :
  AnimateState (CTableView::Instance()),
  pView_       (CTableView::Instance()),
  msecs_       (msecs),
  subtractTime_(0),
  numCards_    (numCards)
{
  if (numCards_ == 1)
  {
    cards_.push_back(Card(suit1, card1));
  }
  else if (numCards_ == 3)
  {
    cards_.push_back(Card(suit1, card1));
    cards_.push_back(Card(suit2, card2));
    cards_.push_back(Card(suit3, card3));
  }
}


StateDealCommunity::StateDealCommunity(
                      int numCards,
                      Network::CardEntry* pCard,
                      int msecs)
  :
  AnimateState (CTableView::Instance()),
  pView_       (CTableView::Instance()),
  msecs_       (msecs),
  subtractTime_(0),
  numCards_    (numCards)
{
  for (int i = 0; i < numCards_; i++, pCard++)
    cards_.push_back(Base::Card(pCard->suit_, pCard->value_));
}


StateDealCommunity::~StateDealCommunity()
{
  Player* pCommunity = pView_->getPlayer(PI_Community);

  if (pCommunity)
  {
    // It can happen that the next deal card
    // pdu comes before this state is done -
    // if so this code takes care of it
    bool cardsLeft = (cards_.size() > 0);

    while (cards_.size() > 0)
    {
      MoveCardToPlayer(cards_, pCommunity);
      pView_->InvalidateRect(
        pCommunity->getCardPaintArea(Player::LastCard));
    }      

    if (cardsLeft)
    {
      ShowCommunityCardMessage(pCommunity);
    }
  }
}


BOOL StateDealCommunity::tick(DWORD dt)
{
  if (subtractTime_ > 0)
  { // Compensate for time spent in flip effect
    if (dt > subtractTime_)
      dt -= subtractTime_;
    else
      dt = 1; // don't wrap!
    subtractTime_ = 0;
  }

  Player* pCommunity = pView_->getPlayer(PI_Community);
  ASSERT(pCommunity);

  if (cards_.size() == 0)
  { //
    // All cards have been dealt - betting starts
    //
    GameLogic::Instance().setNextState(new StateBet());
    return TRUE; 
  }

  BOOL rc = TRUE;

  if (ticks_ == 0)
  { //
    // Start dealing new card
    //
    Player* pHouse = pView_->getHouse();
    ASSERT(pHouse);

    if (!pHouse || !pCommunity) return FALSE; // fatal

    // Get start and end positions for card animation
    pos_ = start_ = pHouse->getCardPos();
    end_ = pCommunity->getCardPos(pCommunity->numCards());

    if (msecs_ > 0)
      startAnimate();
  }

  ticks_ += dt;

  if (ticks_ < msecs_ && msecs_ > 0)
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
    // Dealing animation finished - now actually move
    // the card from house to community
    //
    ticks_ = 0;

    if (msecs_ > 0)
      stopAnimate();

    Player* pHouse = pView_->getHouse();
    ASSERT(pHouse);
    Player* pCommunity = pView_->getPlayer(PI_Community);
    ASSERT(pCommunity);

    if (!pHouse || !pCommunity) return FALSE; // fatal

    pos_ = CPoint(-1, -1);

    if (cards_.size() > 0)
    {
      Card card = MoveCardToPlayer(cards_, pCommunity); 
      pView_->InvalidateRect(
        pCommunity->getCardPaintArea(Player::LastCard));

      if (msecs_ > 0)
      { // Run special animation for showing community cards
        DWORD start = ::timeGetTime();

        CPoint pos = pCommunity->getCardPos(Player::LastCard);
        Cards::Instance().startFlip(card, pos);
        CDC* pDC = pView_->GetDC();
        Cards::Instance().flip(pDC, card, pos);
        pView_->ReleaseDC(pDC);
  
        // Compensate for time spent in effect
        subtractTime_ = (::timeGetTime() - start);
      }
      TRACE1("Showing ccard msg2: %d\n", pCommunity->numCards());
      ShowCommunityCardMessage(pCommunity);
    }
  }

  return TRUE;
}


BOOL StateDealCommunity::draw(CDC*)
{
  return TRUE;
}


BOOL StateDealCommunity::onCommand(WPARAM wParam, LPARAM lParam)
{
  // It is possible that the server sends the next Action
  // Request PDU before this state has finished its job.
  // We handle that case here by finishing up immediately
  // and setting the state to StateBet
  if (LOWORD(wParam) == ID_PDU_ACTIONREQUEST ||
      LOWORD(wParam) == ID_PDU_ACTIONREQUESTSPREADLIMIT)
  {
    Player* pHouse = pView_->getHouse();
    ASSERT(pHouse);
    Player* pCommunity = pView_->getPlayer(PI_Community);
    ASSERT(pCommunity);

    // Clean up the animation if any
    if (pos_ != CPoint(-1, -1))
    {
      CRect r(pos_, CSize(Cards::Width_, Cards::Height_));
      pView_->InvalidateRect(&r);
    }

    if (pHouse && pCommunity)
    {
      TRACE1("Action Request PDU while dealing, %d community cards left.\n", cards_.size());

      bool cardsLeft = cards_.size() > 0;

      while (cards_.size() > 0)
      {
        MoveCardToPlayer(cards_, pCommunity);
        pView_->InvalidateRect(
            pCommunity->getCardPaintArea(Player::LastCard));
      }    
      if (cardsLeft)
      {
        TRACE1("Showing ccard msg3: %d\n", pCommunity->numCards());
        ShowCommunityCardMessage(pCommunity);
      }
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

