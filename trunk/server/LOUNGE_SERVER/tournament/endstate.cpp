/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CEndState
  Created  : 07/25/2000

  OVERVIEW : This state ends a tournament.

 ****************************************************************************/

#include "tournament/endstate.h"
#include "tournament/tournamentevent.h"
#include "tournament/tournament.h"
#include "tournament/basetournamentqueue.h"
#include "dbhandler/dbinterface.h"


CEndState::CEndState(CTournament* t)
    :
    CTournamentState(t)
{
    t->logEvent(TE_Tournament, NULL, "Tournament ends");
}

void CEndState::tick(long now)
{    
    CTournamentState::tick(now);

    tournament_->setIsRunning(false);
    tournament_->setNumPlayers(0);
    tournament_->setNextState(NULL);

    CBaseTournamentQueue* tq = tournament_->getTournamentQueue();
    if (tq->getGame() &&
        tq->getGame()->isTournament())
    {
      // Schedule a new multi-table tournament after
      // N minutes if we're in testing mode
      int testState = 0;
      CdbInterface::Inst()->getTestState(testState);

      if (testState > 0)
      {
        CdbInterface::Inst()->scheduleNewTournament(time(NULL) + 10*60,
                                                    tq->getQueueNumber());
      }
    }

    delete this;
}

