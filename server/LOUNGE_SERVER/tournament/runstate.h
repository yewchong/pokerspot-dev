#ifndef __lounge_runstate_h__
#define __lounge_runstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CRunState
  Created  : 07/10/2000

  OVERVIEW : This state runs a tournament.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "tournament/event.h"
#include "compat.h"
#include "common/chips.h"
#include <list>

class CTable;

enum RunState
{
    RS_Running = 1,
    RS_Dissolve = 2
};


class CRunState : public CTournamentState
{
public:
    CRunState(CTournament*, long);

    virtual void tick(long);
    virtual bool canAddPlayer(const CPlayer&);
    virtual void playerReseat(CTable*, const char*);
    virtual bool allowRelogin() const;

private:
    bool checkEndCondition();
    bool checkDissolve();
    bool isDissolving() const;
    bool checkDissolveRoom(CTable*);
    void dissolveTable(CTable*);

    void run(long);
    void dissolve(long);

    long lastAction_;

    long now_, startTime_;

    CChips lolimit_;
    CChips hilimit_;
    bool frozen_;

    int subState_;
    CTable* table_;

    bool first_;
/*
public:
    static void DumpEvents();
    static void ClearEvents();
    static void AddEvent(CTimedEvent*);
    static void AddEvent(CPlayerEvent*);
    static void AddEvent(CTableEvent*);

private:
    static list<CTimedEvent*> timedEvents_;
    static list<CPlayerEvent*> playerEvents_;
    static list<CTableEvent*> tableEvents_;
*/
    int dissolveCounter_;
};


#endif