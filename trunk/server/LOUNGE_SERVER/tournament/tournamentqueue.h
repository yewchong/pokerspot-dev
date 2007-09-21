#ifndef __lounge_tournamentqueue_h__
#define __lounge_tournamentqueue_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTournamentQueue
  Created  : 07/07/2000

  OVERVIEW : This is a special kind of a waiting list that
             manages multi-table tournaments.

 ****************************************************************************/

#include "tournament/basetournamentqueue.h"

class CTournament;

class CTournamentQueue : public CBaseTournamentQueue
{
public:
    static CTournamentQueue* FindQueue(CTable*);
    static CTournamentQueue* GetMultiTableTournamentQueue(int number);

    void redefineThis() const;

    CTournamentQueue(CGame*    pGame,
                     int       number,
                     u_int16_t gameType,
                     u_int16_t queueMax,
                     u_int16_t tableMax,
                     const CChips& lo,
                     const CChips& hi,
                     int spawnThreshold,
                     int maxTables = MaxTablesDefault);
};


#endif
