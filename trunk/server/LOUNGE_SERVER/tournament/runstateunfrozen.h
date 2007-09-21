#ifndef __lounge_runstateunfrozen_h__
#define __lounge_runstateunfrozen_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CRunStateUnfrozen
  Created  : 07/10/2000

  OVERVIEW : This state runs a tournament.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "psi_win32.h"

class CTable;

class CRunStateUnfrozen : public CTournamentState
{
public:
    CRunStateUnfrozen(CTournament*);

    virtual void tick(long);
    virtual bool canAddPlayer(const CPlayer&);
    virtual bool isUnfrozen() const;
    virtual bool allowRelogin() const;

    long startTime_;
};


#endif