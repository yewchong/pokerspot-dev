#ifndef __lounge_event_h__
#define __lounge_event_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CEvent
  Created  : 08/04/2000

  OVERVIEW : These classes implement different events for tournaments.
             The events can be based on time, number of tables or number
             of players.

 ****************************************************************************/

class CAction;
class CTournament;

class CTimedEvent
{
public:
    CTimedEvent(long dt, CAction* action);
    ~CTimedEvent();

    bool test(CTournament*, long);

private:
    long start_, at_;
    CAction* action_;
};


class CPlayerEvent
{
public:
    CPlayerEvent(int numPlayers, CAction* action);
    ~CPlayerEvent();

    bool test(CTournament*, int);

private:
    int numPlayers_;
    CAction* action_;
};


class CTableEvent
{
public:
    CTableEvent(int numTables, CAction* action);
    ~CTableEvent();

    bool test(CTournament*, int);

private:
    int numTables_;
    CAction* action_;
};


#endif