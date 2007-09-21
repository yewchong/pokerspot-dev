#ifndef __lounge_action_h__
#define __lounge_action_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CAction
  Created  : 08/04/2000

  OVERVIEW : These classes implement different actions for tournaments.

 ****************************************************************************/

#include "tournament/pdusynchronize.h"
#include "lounge/game.h"
#include "common/chips.h"

class CTournament;

class CAction
{
public:
    virtual void tick(CTournament*, long now, long endTime);
    virtual void fire(CTournament*) = 0;
};

class CLimitAction : public CAction
{
public:
    CLimitAction(const CChips& lolimit);    
    void fire(CTournament*);

private:
    CChips lolimit_;
};


class CSyncAction : public CAction
{
public:
    CSyncAction(SyncFlag);
    CSyncAction(SyncFlag, int pauseTime);

    void tick(CTournament*, long now, long endTime);
    void fire(CTournament*);

private:
    SyncFlag syncFlag_;

    long startTime_;
    int  mins_;
    int  pauseTime_;

    bool statusSent_;
    void sendTournamentStatus(CTournament*);
};


class CBetStructureAction : public CAction
{
public:
    CBetStructureAction(int bet);
    void fire(CTournament*);

private:
    int betStructure_;
};


class CGameTypeAction : public CAction
{
public:
    CGameTypeAction(GameType gt, bool hilo);
    void fire(CTournament*);

private:
    GameType gameType_;
    bool     hilo_;
};


#endif