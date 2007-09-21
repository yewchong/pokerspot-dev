#ifndef __lounge_startstate_h__
#define __lounge_startstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CStartState
  Created  : 07/07/2000

  OVERVIEW : This state implements the starting of a tournament.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "tournament/seater.h"

class CStartState : public CTournamentState
{
public:
    CStartState(CTournament*);

    virtual void tick(long);
    virtual bool canAddPlayer(const CPlayer&);

private:
    enum SubState
    {
        Start = 0,
        Spawn = 1,
        Seat  = 2,
        Seating_Step = 3,
        Seated0 = 4,
        Seated1 = 5,
        Seated2 = 6,
        Done   = 7
    };

    int subState_;
    int spawn_, numTables_;
    long lastAction_;
    int skipSteps_;

    CSeater seater_;

    // Seated players
    list<string> seated_;
};


#endif