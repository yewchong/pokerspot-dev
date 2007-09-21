#ifndef __lounge_waitstartstate_h__
#define __lounge_waitstartstate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitStartState
  Created  : 07/07/2000

  OVERVIEW : Tournament wait start state.

 ****************************************************************************/

#include "tournament/tournamentstate.h"
#include "psi_win32.h"

class CTournament;

class CWaitStartState : public CTournamentState
{
public:
  CWaitStartState(CTournament*);

  virtual void tick(long);
  virtual bool canAddPlayer(const CPlayer&);

private:
  bool chatDisabled_;
};


#endif