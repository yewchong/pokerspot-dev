/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitState
  Created  : 08/02/2000

  OVERVIEW : This state waits for an event to occur.

 ****************************************************************************/

#include "tournament/waitstate.h"
#include "tournament/waitpredicate.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "lounge/table.h"
#include "pdu/pduqueuefull.h"
#include <stdio.h>

// 03/09/2001 Tournament table dissolve bug fix.
// While a tournament table merge is in progress,
// the player must not be allowed to sit back at his
// table because that could interfere with the merge
// process causing the dissolve to fail and ultimately
// hang the whole tournament!

bool TournamentTableDissolveIsInProgress(CTournament*)
{
  return false;
}

CWaitState::CWaitState(CTournament* t,
                       CTournamentState* nextState,
                       CWaitPredicate*   waiter)
  :
  CTournamentState(t),
  nextState_      (nextState),
  waiter_         (waiter),// owned
  tableDissolveInProgress_(true)
{}
    

CWaitState::~CWaitState()
{
  tableDissolveInProgress_ = false;
  delete waiter_;
  waiter_ = NULL;
}


void CWaitState::tick(long now)
{
    CTournamentState::tick(now);

    printf("CWaitState waiting %i\n", now);

    if (waiter_->done(now))
    {
        printf("CWaitState Completed\n");
        setNextState(nextState_);
        delete this;
    }
}


bool CWaitState::canAddPlayer(const CPlayer& p)
{
    // Don't allow relogins while seating takes place
    CpduQueueFull pdu;
    pdu.sendQueueFull(tournament_->getTournamentQueue()->getQueueNumber(),
                      const_cast<CPlayer*>(&p),
                      QF_TournamentSeating);
    return false;
}


void CWaitState::playerReseat(CTable*     table,
                              const char* username)
{
    printf("CWaitState::playerReseat: %s %s\n",
           table->getTitle().c_str(), username);

    dumpTables();

    if (waiter_)
        waiter_->playerReseat(table, username);
}


bool CWaitState::tableDissolveInProgress() const
{
  return true;
}