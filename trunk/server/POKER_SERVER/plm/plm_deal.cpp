/**********************************************************************

  NAME:		plm_deal.cpp
  DATE:		4/8/00
  PURPOSE:	Class Cplm implements the methods needed to actually run a
			poker game.  PLM stands for "Poker Logic Module", which is
			exactly what this class strives to implement: the logic to
			run a poker game.

**********************************************************************/

#include "plm.h"
#include "ring/ring.h"
#include "table/table.h"
#include "player/player.h"

// Deal cards to active players. 
void Cplm::doDealPocketCards(int numCards)
{
  if (table_->getGameType() == Game_Type_SevenStud)
  {
    if (numCards == 1)
    {   // Dealing the final pocket card - must
      // check for the deck-out-of-cards situation
      // with 8 players playing all the way to the
      // end
      if (table_->getDeckNumCards() < table_->countPlayers(PLAYER_STATE_PLAYING|
                                                           PLAYER_STATE_ALLIN |
                                                           PLAYER_STATE_ALLIN_RAISE))
      {
        // The last card must be dealt as a community card
        doDealCommunityCards(1);
        return;
      }
    }
  }
  
  // Deal cards starting from next to button
  CRingIterator it(table_->getDealer());
  ++it;
  // For every player that is PLAYER_STATE_PLAYING, send cards..
  // For those sitting out, send dummy cards so they see the action too
  int state = PLAYER_STATE_PLAYING |
              PLAYER_STATE_FOLDED	|
              PLAYER_STATE_ALLIN |
              PLAYER_STATE_ALLIN_RAISE |
              PLAYER_STATE_SITOUT;

  if (it.restart(state))
  {
    do
    {
      if (it.current()->getPlayer()->matchState(PLAYER_STATE_PLAYING|
                                                PLAYER_STATE_ALLIN |
                                                PLAYER_STATE_ALLIN_RAISE))
      { // Send real cards to this player
        table_->dealCards(it.current()->getPlayer(), numCards);
      }
      else
      { // Send dummy cards to the rest so they see the action
        CpduDealCardsFlex pdu;
        pdu.sendDummyDealCards(it.current()->getPlayer(), numCards);
      }
    }
    while (it.next(state));
    
    // Send dummy deal cards to observers
    SingleLock l(&table_->getTableMutex());
    l.lock();
    
    for (int i = 0; i < CTable::MaxObservers; i++)
    {
      CPlayer* p = table_->getObserver(i);
      if (p && (p->getConnfd() != -1))
      {
        CpduDealCardsFlex pdu;
        pdu.sendDummyDealCards(p, numCards);
      }
    }
    
    // Sleep a bit to allow clients to show animation 
    Sys_Sleep(1000);
  }
}

void Cplm::doDealCommunityCards(int numCards)
{
    table_->dealCards(table_->getRing(), numCards);

    if (table_->getBoard() &&
        table_->getBoard()->getNumCards() == 3)
    {
        // The flop was dealt
        table_->updateFlopPercentage();
    }
};

// 7-stud
void Cplm::doDealUpCards(int numCards) 
{
    upCards_++;

    if (upCards_ == 2)
    {
      // In 7-Stud there is no flop. The corresponding %
      // is computed at second upcard.
      table_->updateFlopPercentage();
    }

    // Deal cards starting from next to button
    CRingIterator it(table_->getDealer());
    ++it;
    // For every player that is PLAYER_STATE_PLAYING, send cards..
    // For those sitting out, send dummy cards so they see the action too
    int state = PLAYER_STATE_PLAYING|PLAYER_STATE_ALLIN|PLAYER_STATE_SITOUT;
    if (it.restart(state))
    {
        do
        {
            if (it.current()->getPlayer()->matchState(PLAYER_STATE_PLAYING|
						      PLAYER_STATE_ALLIN))
            {   // Send real cards to this player
                table_->dealUpCards(it.current()->getPlayer(), numCards);
                //Sys_Sleep(500); // XXX 7-stud
            }
        }
        while (it.next(state));


        Sys_Sleep(1000);
    }
};
