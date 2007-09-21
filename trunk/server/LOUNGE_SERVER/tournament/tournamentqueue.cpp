/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTournamentQueue
  Created  : 07/07/2000

  OVERVIEW : This is a special kind of a waiting list that
             manages tournaments.

 ****************************************************************************/

#include "tournament/parser.h"
#include "tournament/tournamentqueue.h"
#include "tournament/tournament.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/tournamentevent.h"
#include "tournament/pduseatplayer.h"
#include "lounge/queueservice.h"
#include "lounge/game.h"
#include "lounge/lounge.h"
#include "pdu/pduqueueupdate.h"
#include "pdu/pduqueuefull.h"
#include "pdu/pdunotify.h"
#include "dbhandler/dbinterface.h"
#include "common/chips.h"
#include "sys.h"

#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif


CTournamentQueue::CTournamentQueue(
                     CGame*    pGame,
                     int       number,
                     u_int16_t gameType,
                     u_int16_t queueMax,
                     u_int16_t tableMax,
                     const CChips& lo,
                     const CChips& hi,
                     int spawnThreshold,
                     int maxTables)
    :
    CBaseTournamentQueue(pGame, number, gameType,
                         queueMax, tableMax, lo, hi,
                         spawnThreshold, maxTables)
{}

CTournamentQueue* CTournamentQueue::GetMultiTableTournamentQueue(int index)
{
  CBaseTournamentQueue* tq = FindQueueByIndex(index);
  if (tq)
  {
    CGame* game = tq->getGame();
    if (game && game->isTournament()) // i.e., multitable tournament, not satellite
    {
      // safe downcast
      return (CTournamentQueue*)tq;
    }
  }
  return NULL;
}

void CTournamentQueue::redefineThis() const
{}

