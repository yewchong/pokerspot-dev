/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUDealCardsFlex
  Created  : 04/11/2000

  OVERVIEW : Flexible Deal Cards PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pdudealcardsflex.h"
#include "base/statedealpocket2.h"
#include "base/statedealcommunity.h"
#include "base/gamelogic.h"
#include "base/gamestate.h"
#include "base/player.h"
#include "base/card.h"
#include "base/statedealupcard.h"
#include "ui/tableview.h"
#include "ui/chatview.h"
#include "ui/cmdid.h"
#include "ui/global.h"

#ifdef _DEBUG
  #define new DEBUG_NEW
  #undef THIS_FILE
  static char THIS_FILE[] = __FILE__;
#endif


BEGIN_NAMESPACE(Network)

using Base::GameLogic;

PDUDealCardsFlex::PDUDealCardsFlex(const char* buf)
  :
  PDUHeader (buf),
  player_slot_(0),
  card_type_  (0),
  num_cards_  (0),
  msecs_      (100),
  pCards_     (NULL)
{  
  u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
  player_slot_ = ntohs(*pint++);
  card_type_ = ntohs(*pint++);
  num_cards_ = ntohs(*pint++);

  // milliseconds are hard-coded for now
  //  msecs_ = ntohs(*pint++);

  // Rest of the data is CardEntries - extract
  // from the buffer
  size_t extraBytes = length_ - sizeOf();
  u_int16_t num_cards = extraBytes / sizeof(CardEntry);
  if (card_type_ != CT_Dummy)
  {
    PDU_ASSERT(num_cards == num_cards_);
    num_cards_ = num_cards;
    if (num_cards_ > 0)
      pCards_ = new CardEntry[num_cards_];
  }

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
}


PDUDealCardsFlex::~PDUDealCardsFlex()
{
  delete [] pCards_;
  pCards_ = NULL;
}
  

void PDUDealCardsFlex::execute(GameState*)
{
  CString msg;
  CTableView* pTable = CTableView::Instance();
  ASSERT(pTable);
  if (!pTable) return;

  Player* pPlayer = pTable->getPlayer(player_slot_);

  switch (card_type_)
  {
    case CT_Dummy:
    {
      GameLogic::CardsDealt();

      // deal hole cards to other players (down)
      GameLogic::Instance().setNextState(new StateDealPocket2(
                                                pTable->dealer(),
                                                num_cards_,
                                                msecs_));
      break;
    }

    case CT_Hole:
    {
      if (player_slot_ == 0xffff)
      {
        GameLogic::CardsDealt();

        if (Global::GetGameType() == GT_SevenStud)
        {
          GameState* state = GameLogic::Instance().currentState();
          if (state)
          {
            // Tell the current state to quit animations if any
            state->onCommand(MAKEWPARAM(ID_STOPANIMATION, 0), 0);
          }

          if (num_cards_ == 1)
          { // last pocket card being dealt, betting round ends
            pTable->endBettingRound();
          }
        }

        // deal hole cards to local player
        GameLogic::Instance().setNextState(new StateDealPocket2(
                                                 pTable->dealer(),
                                                 num_cards_,
                                                 pCards_,
                                                 msecs_));
        break;
      }
      // fall through intended!
    }

    case CT_Indian:
    {
      // Check if we already have a DealUpCard state active
      GameState* state = GameLogic::Instance().currentState();
      if (state)
      {
        // Tell the current state to quit animations if any
        state->onCommand(MAKEWPARAM(ID_STOPANIMATION, 0), 0);

        // Tell state new pdu has arrived
        state->onCommand(MAKEWPARAM(ID_PDU_DEALUPCARD, 0), 0);

        BOOL isDealUpCards =
          state->onCommand(MAKEWPARAM(ID_ISDEALUPCARD, 0), 0);
        if (!isDealUpCards)
          state = NULL;
      }

      StateDealUpCard* pS = NULL;

      if (!state)
      { // First up card, create state
        pS = new StateDealUpCard(player_slot_, num_cards_, pCards_, 2*msecs_);
        GameLogic::Instance().setNextState(pS);
      }
      else
      { // Another up card, restart state 
        pS = static_cast<StateDealUpCard*>(state);
        pS->dealUpCard(player_slot_, num_cards_, pCards_, 2*msecs_);
      }

      break;
    }

    case CT_Community:
    {
      // deal community cards
      GameLogic::Instance().setNextState(new StateDealCommunity(
                                               num_cards_,
                                               pCards_,
                                               msecs_));
    }
    break;
  }
}



END_NAMESPACE(Network)