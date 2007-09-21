#ifndef __base_suit_h__
#define __base_suit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : enum Base::Suit
  Created  : 24.11.1999

  OVERVIEW : Enumerated type for representing Suits.

 ****************************************************************************/

#include "network/compat.h"

BEGIN_NAMESPACE(Base)

  enum Suit
  {
    SuitBegin = 0,
    Clubs     = 1,
    Diamonds  = 2,
    Hearts    = 3,
    Spades    = 4,
    SuitEnd   = 5
  };

END_NAMESPACE(Base)


#endif
