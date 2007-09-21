/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : plm_rake.cpp
  Created  : 1/1/2001

  OVERVIEW : Rake related Cplm mfunctions.

 ****************************************************************************/

#include "plm.h"
#include "table/table.h"
#include "pot/pot.h"
#include "dbinterface/dbinterface.h"
#include "tournament/tournament.h"

#ifndef _WIN32
u_int16_t min(int i1, u_int16_t i2)
{
    return i1 < i2 ? i1 : i2;
}
#endif

/*
  doRake implements the following rake rules:


    OLD Rake structure:
    For 2-4 and 3-6, there's a maximum rake of $2.  $1 at the flop,
    and $1 when the pot reaches $20.
    For 5-10+ limits, the rake is $1 at the flop, $1 when the pot
    reaches $40, and $1 when it reaches $60.
    When there are only 2-3 players at a table (including all 1-1 games),
    then there is a maximum of $1 rake at the flop.

    NEW rake as of 060200:
    For 2/4 and 3/6 there is a maximum rake of $2.  $1 at the flop and $1 when
    the pot reaches $20.

    For $5/10 and up, the rake is $1 at the flop, $1 when the pot reaches $40,
    and $1 when the pot reaches $60.

    When there are only 2 or 3 players at any table (including 1-1 games),
    there is a $1 maximum rake when the pot reaches $20.
*/

void Cplm::resetRake()
{
  rake_ = 0;
  flopRake_ = false;

  switch (table_->getLo().getDollars())
  {
  case 1: 
  case 2:
  case 3:
  case 4: 
    lowRake_ = 20;
    hiRake_ = 0; // no hi rake in 2-4, 3-6
    break;

  default: // 5-10+ limits:
    lowRake_ = 40;
    hiRake_ = 60;
    break;
  }
}

//
// Take the rake according to the rules.
//

void Cplm::doRake()
{
  if (lowRake_ == 0 && hiRake_ == 0)
    resetRake(); // set the rake limits

  Cpot* pot = table_->getMainPot();

  bool flop = false;    

// RULE:
//      $1 at the flop
  if (table_->getBoard() &&
      table_->getBoard()->getNumCards() >= 3)
  {   // 
      // Take flop rake
      //
      rake_ = 1;
      flop = true;
  }

// RULE:
//    $1 when the pot reaches certain low limit
  if (flop && lowRake_ != 0)
  {
    if (pot->getTotalChips() >= lowRake_)
    {   //
        // Take low limit rake
        //
        rake_ += 1;
    }
  }

// RULE:
//    $1 when the pot reaches certain higher limit
  if (flop && hiRake_ != 0)
  {
    if (pot->getTotalChips() >= hiRake_)
    {   //
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
      if (flop)
        rake_ = minimum(CChips(1, 0), rake_);
    }
  }

  // In tournaments there is no rake
  if (CTournament::Inst()->isTournament())
  {
    rake_ = 0;
  }
  
  saveRake();
}


// 
// Take rake chips from main pot, save rake to
// database, zero out old rake.
//
void Cplm::saveRake()
{
  Cpot* pot = table_->getMainPot();

  CChips rake = rake_;
  rake_ = 0;

  ASSERT(rake <= pot->getChips());

  char message[100];
  sprintf(message, "Pot: $%9.2f | Rake: $%9.2f", 
          pot->getTotalChips().asDouble(), rake.asDouble());

  if (rake > 0)
    table_->getDbase()->saveRake(rake);

  table_->setLog(message);
  pot->subtractChips(rake);
}
