/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitStartState
  Created  : 07/07/2000

  OVERVIEW : Tournament wait start state. This state waits
             until it is time to start the tournament.

 ****************************************************************************/

#include "tournament/waitstartstate.h"
#include "tournament/startstate.h"
#include "tournament/tournament.h"
#include "pdu/pduchatenable.h"


CWaitStartState::CWaitStartState(CTournament* t)
  :
  CTournamentState(t),
  chatDisabled_   (false)
{}

void CWaitStartState::tick(long now)
{
  CTournamentState::tick(now);

  if (!chatDisabled_ &&
      now > (tournament_->getStartTime() - 60))
  {
    // Disable chat one minute before seating starts
    chatDisabled_ = true;
    CpduChatEnable pdu;
    pdu.broadcastChatEnable(CE_TournamentSeating, NULL);
  }

  if (now > tournament_->getStartTime())
  {
    tournament_->setIsRunning(true);
    setNextState(new CStartState(tournament_));
    delete this;
  }
}


bool CWaitStartState::canAddPlayer(const CPlayer&)
{
    return true;
}