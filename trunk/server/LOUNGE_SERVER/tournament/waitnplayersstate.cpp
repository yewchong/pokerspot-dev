/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitNPlayersState
  Created  : 12/19/2000

  OVERVIEW : Tournament wait start state - this state
             waits for N players to enter the waiting
             list.

 ****************************************************************************/

#include "tournament/waitnplayersstate.h"
#include "tournament/startstate.h"
#include "tournament/tournament.h"
#include "tournament/satellitequeue.h"


CWaitNPlayersState::CWaitNPlayersState(int numPlayers,
                                       CSatelliteQueue* q,
                                       CTournament* t)
    :
    CTournamentState(t),
    waitingList_    (q),
    numPlayers_     (numPlayers)
{}

void CWaitNPlayersState::tick(long now)
{
    CTournamentState::tick(now);

    if (waitingList_->numPlayers() >= numPlayers_)
    {
        tournament_->setIsRunning(true);
        setNextState(new CStartState(tournament_));
        delete this;
    }
}


bool CWaitNPlayersState::canAddPlayer(const CPlayer&)
{
    return waitingList_->numPlayers() < numPlayers_;
}