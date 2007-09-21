/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CSatelliteQueue
  Created  : 12/12/2000

  OVERVIEW : This is a special kind of a waiting list that
             manages satellite tournaments.

 ****************************************************************************/

#include "tournament/parser.h"
#include "tournament/satellitequeue.h"
#include "tournament/tournament.h"
#include "tournament/waitnplayersstate.h"
#include "lounge/queueservice.h"
#include "pdu/pduqueueupdate.h"
#include "dbhandler/dbinterface.h"
#include "common/chips.h"
#include "sys.h"

#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif

CSatelliteQueue::CSatelliteQueue(
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
                         spawnThreshold, maxTables),
// satellites are single-table tournaments,
// thus 10 players
    maxPlayersInTournament_(10) 
{
  CQueueService::Schedule(this);    
}


void CSatelliteQueue::redefineThis() const
{}

int CSatelliteQueue::tick(long now)
{
  if (!isOwner())
  {
    // This waiting list is owned by another Lounge
    // Server instance
    return 0;
  }

  if (tournament_ && tournament_->hasEnded() &&
      getNumTables() == 0)
  {
      // Previous tournament has ended, delete it
      // and new one will be prepared
      CpduQueueUpdate pdu;
      pdu.sendQueueRemove(getQueueNumber(), tournament_);

      delete tournament_;
      tournament_ = NULL;
      return 0;
  }

  if (!tournament_)
  {
    // Create new tournament

    int number = 1,
        numPlayers = 10;
    CChips buyin = 0,
           startChips = 500,
           startLo = 5;
              
    string descr, script;

    if (CdbInterface::Inst()->getSatellite(
                                queueNumber_,
                                number,
                                buyin,
                                startChips,
                                startLo,
                                numPlayers,
                                descr,
                                script))
    {
      tournament_ = new CTournament(
                          this,
                          number,
                          numPlayers,
                          buyin,
                          startChips,
                          startLo,
                          0, // no start time
                          descr,
                          script);

      if (!tournament_)
      {
        Sys_LogError("Out of memory(new CTournament)!");
        return 0;
      }

      CParser parser(tournament_);
      parser.loadScript(script.c_str());

      CpduQueueUpdate pdu;
      pdu.sendQueueAdd(getQueueNumber(), tournament_);
     
      tournament_->setNextState(new CWaitNPlayersState(
                                      numPlayers,
                                      this,
                                      tournament_));
    }
  }

  return CBaseTournamentQueue::tick(now);
}


bool CSatelliteQueue::canAddPlayer(CPlayer& p)
{
  return CBaseTournamentQueue::canAddPlayer(p);
}


bool CSatelliteQueue::addPlayer(CPlayer& p)
{
  return CBaseTournamentQueue::addPlayer(p);
}
