#ifndef __lounge_waitstate_h__
#define __lounge_waitstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitState
  Created  : 08/02/2000

  OVERVIEW : This state waits for an event to occur.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "psi_win32.h"

class CWaitPredicate;

class CWaitState : public CTournamentState
{
public:
    CWaitState(CTournament*      tournament,
               CTournamentState* nextState,
               CWaitPredicate*   waiter);
    ~CWaitState();

    virtual void tick(long);
    virtual bool canAddPlayer(const CPlayer&);
    virtual void playerReseat(CTable*,
                              const char* username);
    virtual bool tableDissolveInProgress() const;

private:
    CTournamentState* nextState_;
    CWaitPredicate*   waiter_;
    bool tableDissolveInProgress_;
};


#endif