/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUCommunityCard
  Created  : 10.11.1999

  OVERVIEW : Community Card PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pducommunitycard.h"
#include "base/gamelogic.h"
#include "base/player.h"
#include "base/cards.h"
#include "base/suit.h"
#include "base/misc.h"
#include "base/statedealcommunity.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/soundmanager.h"
#include "ui/cmdid.h"

using Base::GameLogic;
using Base::GameState;
using Base::Cards;
using Base::Card;
using Base::AsString;

// Show a dealer chat message for community cards
void ShowCommunityCardMessage(Player* pCommunity)
{
#ifdef PCLIENT_MODULE_
  CChatView* pChat = CChatView::Instance();
  if (pChat)
  {
    CString s;
    // Add a message to the chat window
    if (pCommunity->numCards() == 3)
    { // Flop was dealt
      CString s1, s2, s3;
      Card c1, c2, c3;
      pCommunity->getCard(0, c1);
      pCommunity->getCard(1, c2);
      pCommunity->getCard(2, c3);

      s1 = AsString(c1);
      s2 = AsString(c2);
      s3 = AsString(c3);
      s.Format("Flop is %s, %s, %s",
               (LPCTSTR)s1, (LPCTSTR)s2, (LPCTSTR)s3);
    }
    else if (pCommunity->numCards() == 4)
    {
      CString s1;
      Card c;
      pCommunity->getCard(3, c);
  
      s1 = AsString(c);
      s.Format("Turn is %s", (LPCTSTR)s1);
    }
    else if (pCommunity->numCards() == 5)
    {
      CString s1;
      Card c;
      pCommunity->getCard(4, c);
  
      s1 = AsString(c);
      s.Format("River is %s", (LPCTSTR)s1);
    }
    if (!s.IsEmpty())
    {
      pChat->addDealerMessage(s, CChatView::CF_GameNormal);
    }
  }
#endif
}


BEGIN_NAMESPACE(Network)


void PDUCommunityCard::execute(Base::GameState*)
{
  // Note: the StateDealCommunity is not used since
  // if a new community card pdu is received while a
  // StateDealCommunity is active then the previous
  // community card is lost. Instead, this PDU puts
  // the card directly to the community player.
    
  CTableView* pView = CTableView::Instance();
  ASSERT_VALID(pView);

  if (pView)
  {

    // Tell the current state to quit animations if any
    GameState* state = GameLogic::Instance().currentState();
    if (state)
      state->onCommand(MAKEWPARAM(ID_STOPANIMATION, 0), 0);

    pView->endBettingRound();

    // Update the screen now to make sure whatever we
    // were doing becomes visible
    pView->UpdateWindow();

    Player* pCommunity = pView->getCommunity();
    ASSERT(pCommunity);

    if (pCommunity)
    { // Add card directly to community's deck
      Card card(suit_, card_);
      pCommunity->addCard(card);

      SndMan::Instance()->playSound(SndMan::SM_Card2);

      if (msecs_ > 0)
      {
        // Do the "flip" animation to show the card
        CPoint pos = pCommunity->getCardPos(Player::LastCard);
        Cards::Instance().startFlip(card, pos);
        CDC* pDC = pView->GetDC();
        if (pDC)
        {
          Cards::Instance().flip(pDC, card, pos);
          pView->ReleaseDC(pDC);
        }
      }
      else
      {
        // Just repaint it once
        pView->InvalidateRect(pCommunity->getCardPaintArea(Player::LastCard));
      }

      ShowCommunityCardMessage(pCommunity);
    }
  }  
}

END_NAMESPACE(Network)