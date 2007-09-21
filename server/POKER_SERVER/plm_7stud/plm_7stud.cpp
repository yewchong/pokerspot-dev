/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Class    : Cplm_7Stud
  Created  : 06/21/2000

  OVERVIEW : 7 Card Stud logic module. This is a subclass
             of the basic Cplm that implements 7 Card Stud
             specific ante and betting structure.

 ****************************************************************************/

#include "plm_7stud.h"
#include "table/table.h"


Cplm_7Stud::Cplm_7Stud(CTable* table)
  :
  Cplm(table),
  bettingRound_(0)
{}


Cplm_7Stud::~Cplm_7Stud()
{}


void Cplm_7Stud::doCleanup()
{
  // Let default implementation do the work
  Cplm::doCleanup();

  bettingRound_ = 0;
}


void Cplm_7Stud::doRake()
{
  if (lowRake_ == 0 && hiRake_ == 0)
    resetRake(); // set the rake limits
  
  Cpot* pot = table_->getMainPot();
  
  bool secondUpCard = false;    
  
  // RULE:
  //      $1 at the second upcard
  if (upCards_ >= 2)
  { // 
    // Take second up card rake
    //
    rake_ = 1;
    secondUpCard = true;
  }
  
  // RULE:
  //    $1 when the pot reaches certain low limit
  if (secondUpCard && lowRake_ != 0)
  {
    if (pot->getTotalChips() >= lowRake_)
    { //
      // Take low limit rake
      //
      rake_ += 1;
    }
  }
  
  // RULE:
  //    $1 when the pot reaches certain higher limit
  if (secondUpCard && hiRake_ != 0)
  {
    if (pot->getTotalChips() >= hiRake_)
    { //
      // Take hi limit rake
      //
      rake_ += 1;
    }
  }
  
  // RULE
  //   If there are only 2-3 players playing (that is,
  //   has been dealt cards), the rake is 1 if pot reaches
  //   20, otherwise rake is 0.
  
  const int NoRakeLimit = 20;
  
  // Players in these states have been dealt cards
  int state = PLAYER_STATE_PLAYING |
              PLAYER_STATE_FOLDED |
              PLAYER_STATE_ALLIN |
              PLAYER_STATE_ALLIN_RAISE;
  
  if (table_->countPlayers(state) < 4)
  {   
    if (pot->getTotalChips() < NoRakeLimit)
    {
      // NoRakeLimit not reached, no rake
      rake_ = 0;
    }
    else
    {
      // NoRakeLimit reached
      if (secondUpCard)
        rake_ = minimum(CChips(1, 0), rake_);
    }
  }
  
  saveRake();
}


// In 7-stud, the odd chip goes to player with
// highest card of suit in this order:
// Spades, Hearts, Diamonds, Clubs.
CPlayer* Cplm_7Stud::getOddChipWinner(Winners& winners)
{
  CPlayer* player = NULL;
  
  u_int16_t highestCard = 0;
  
  for (Winners::iterator it = winners.begin(),
                         end = winners.end();
       it != end;
       ++it)
  {
    CWinner& winnerEntry = const_cast<CWinner&>(*it);
    CPlayer* winner = winnerEntry.player_;
    
    if (winner && winner->hasHand())
    {
      Chand* hand = winner->getHand();
      if (hand)
      {
        CCard* card = hand->getCard();
        while (card != NULL)
        {
          u_int16_t thisCard = card->getSuit() * 13 + card->getValue();
          if (thisCard > highestCard)
          {
            highestCard = thisCard;
            player = winner;
          }
          card = card->getNext();
        }
      }
    }
  }
  
  if (!player)
    player = getPlayerClosestToLeftOfDealer(winners);
  
  return player;
}