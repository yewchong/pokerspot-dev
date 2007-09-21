#ifndef __base_playerstate_h__
#define __base_playerstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : Enumeration PlayerState
  Created  : 03/31/2000

  OVERVIEW : Possible states in which a player can be.

 ****************************************************************************/


BEGIN_NAMESPACE(Base)

  enum State
  {
    Joined     = 1, 
    Left       = 2,
    SitIn      = 4,
    SitOut     = 8,
    Fold       = 16,
    ChipUpdate = 32,
    AllIn      = 64,
    BoughtIn   = 128,
    Zombie     = 256
  };

END_NAMESPACE(Base)

#endif