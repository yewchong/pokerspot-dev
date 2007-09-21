#ifndef __lounge_satellitequeue_h__
#define __lounge_satellitequeue_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CSatelliteQueue
  Created  : 12/12/2000

  OVERVIEW : This is a special kind of a waiting list that
             manages satellite tournaments.

 ****************************************************************************/

#include "tournament/basetournamentqueue.h"


class CSatelliteQueue : public CBaseTournamentQueue
{
public:
    void redefineThis() const;

    CSatelliteQueue(CGame*    pGame,
                    int       number,
                    u_int16_t gameType,
                    u_int16_t queueMax,
                    u_int16_t tableMax,
                    const CChips& lo,
                    const CChips& hi,
                    int spawnThreshold,
                    int maxTables = MaxTablesDefault);

    // CBaseTournamentQueue overrides
    virtual int tick(long now);
    virtual bool canAddPlayer(CPlayer&);
    virtual bool addPlayer(CPlayer&);

private:
    int maxPlayersInTournament_;
};


#endif
