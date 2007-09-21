#ifndef __lounge_tournamentstate_h__
#define __lounge_tournamentstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTournamentState
  Created  : 07/07/2000

  OVERVIEW : Base class for tournament states.

 ****************************************************************************/

#include "psi_win32.h"
#include "compat.h"
#include <list>
#include <string>
USING_NS_T(std, string);
USING_NS_T(std, list);

class CTournament;
class CPlayer;
class CTable;

class CTournamentState
{
public:
    CTournamentState(CTournament*);
    virtual ~CTournamentState();
    virtual void tick(long);

    void setNextState(CTournamentState*);

    virtual bool canAddPlayer(const CPlayer&);
    virtual void playerReseat(CTable*, const char*);
    virtual void waitReseat(const list<string>& players);
    virtual bool isUnfrozen() const;
    virtual bool allowRelogin() const;
    virtual bool tableDissolveInProgress() const;

    void dumpTables();

protected:
    CTournament* tournament_;

    int ticks_;
};


#endif