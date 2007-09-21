#ifndef __lounge_endstate_h__
#define __lounge_endstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CEndState
  Created  : 07/25/2000

  OVERVIEW : This state ends a tournament.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "psi_win32.h"

class CEndState : public CTournamentState
{
public:
    CEndState(CTournament*);

    virtual void tick(long);
};


#endif