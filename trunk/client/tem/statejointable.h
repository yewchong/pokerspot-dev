#ifndef __the_statejointable_h__
#define __the_statejointable_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateJoinTable
  Created  : 9.11.1999

  OVERVIEW : Implements joining to a table for Texas Hold'em.

 ****************************************************************************/


#include "base/gamestate.h"
#include "network/network.h"


class StateJoinTable : public Base::GameState
{
public:
  StateJoinTable();

  BOOL tick (DWORD);
  BOOL draw (CDC*);
};


#endif
