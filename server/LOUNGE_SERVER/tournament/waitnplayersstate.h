#ifndef __lounge_waitnplayersstate_h__
#define __lounge_waitnplayersstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitNPlayersState
  Created  : 12/19/2000

  OVERVIEW : Tournament wait start state - this state
             waits for N players to enter the waiting
             list.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "psi_win32.h"

class CTournament;
class CSatelliteQueue;

class CWaitNPlayersState : public CTournamentState
{
public:
    CWaitNPlayersState(int numPlayers,
                       CSatelliteQueue*,
                       CTournament*);

    virtual void tick(long);
    virtual bool canAddPlayer(const CPlayer&);

private:
  int numPlayers_;
  CSatelliteQueue* waitingList_;
};


#endif