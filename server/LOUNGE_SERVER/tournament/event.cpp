/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CEvent
  Created  : 08/04/2000

  OVERVIEW : These classes implement different events for tournaments.
             The events can be based on time, number of tables or number
             of players.

 ****************************************************************************/

#include "tournament/event.h"
#include "tournament/action.h"

// class CTimedEvent

CTimedEvent::CTimedEvent(long at, CAction* action)
    :
    start_ (0),
    at_    (at),
    action_(action) // owned
{}

CTimedEvent::~CTimedEvent()
{
    delete action_;
    action_ = NULL;
}

bool CTimedEvent::test(CTournament* t, long now)
{
    bool rc = false;

    if (start_ == 0)
        start_ = now;

    if (action_)
        action_->tick(t, now, at_);
    
    printf("CTimedEvent: elapsed %i at %i\n",
           now - start_, at_);

    if (now - start_ >= at_)
    {
        action_->fire(t);
        rc = true;
    }

    return rc;
}

// class CPlayerEvent

CPlayerEvent::CPlayerEvent(int numPlayers, CAction* action)
    :
    numPlayers_(numPlayers),
    action_    (action)
{}


CPlayerEvent::~CPlayerEvent()
{
    delete action_;
    action_ = NULL;
}

bool CPlayerEvent::test(CTournament* t, int numPlayers)
{
    bool rc = false;

    if (numPlayers <= numPlayers_)
    {
        action_->fire(t);
        rc = true;
    }

    return rc;
}


// class CTableEvent

CTableEvent::CTableEvent(int numTables, CAction* action)
    :
    numTables_(numTables),
    action_   (action)
{}

CTableEvent::~CTableEvent()
{
    delete action_;
    action_ = NULL;
}

bool CTableEvent::test(CTournament* t, int numTables)
{
    bool rc = false;

    if (numTables <= numTables_)
    {
        action_->fire(t);
        rc = true;
    }

    return rc;
}

