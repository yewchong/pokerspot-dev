/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CRunState
  Created  : 07/10/2000

  OVERVIEW : This state runs a tournament.

 ****************************************************************************/

#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif
#include "tournament/runstate.h"
#include "tournament/endstate.h"
#include "tournament/pdutournamentparams.h"
#include "tournament/pdusynchronize.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "tournament/tournamentevent.h"
#include "tournament/pdudissolvetable.h"
#include "lounge/lounge.h"
#include "sys.h"

#define DISSOLVE_TIME (15)

CRunState::CRunState(CTournament* t, long lastAction)
	:
  CTournamentState(t),
	lastAction_		  (lastAction),
  now_            (0),
  startTime_      (0),
  lolimit_        (t->getStartLo()),
  hilimit_        (2 * t->getStartLo()),
  frozen_         (false),
  subState_       (RS_Running),
  table_          (NULL),
  first_          (true),
  dissolveCounter_(5) // check dissolve every Nth iterator
{}


void CRunState::tick(long now)
{
  CTournamentState::tick(now);
  
  if (first_)
  {
    tournament_->dumpEvents();
    
    first_ = false;
    CpduSynchronize pdu;
    pdu.sendSync(tournament_->getTournamentQueue(),
                 SF_Play);
  }
  
  if (subState_ == RS_Running)
    run(now);
  else if (subState_ == RS_Dissolve)
    dissolve(now);
}


bool CRunState::checkEndCondition()
{
  bool rc = false;
  
  CBaseTournamentQueue* tq =
    tournament_->getTournamentQueue();
  ASSERT(tq);
  
  if (tq->countPlayers() < 2 || tq->getNumTables() == 0)
  {
    // We have the winner - tournament ends
    rc = true;
  }
  
  return rc;
}


bool CRunState::checkDissolve()
{
  // Do this only every nth iteration
  if (dissolveCounter_-- != 0)
    return false;
  dissolveCounter_ = 5;
  
  if (isDissolving())
    return false;
  
  CBaseTournamentQueue* tq =
    tournament_->getTournamentQueue();
  if (!tq)
  {
    Sys_LogError("CRunState::checkDissolve: No tournament queue");
    return false;
  }
  
  bool rc = false;
  
  // if it is one of the tournament tables, check
  // dissolve condition
  int numPlayers = tq->countPlayers();
  
  printf("Dissolve: players %i, tables %i, max %i\n",
         numPlayers, tq->getNumTables(), tq->getTableMax());
  
  if (tq->getNumTables() > 1 &&
      numPlayers <= (tq->getTableMax() * tq->getNumTables() - tq->getTableMax()))
  {
    printf("Time to dissolve\n");
    // Time to dissolve the table with least players
    CTable* table = tq->findTableWithLeastPlayers();
    if (table && !table->isDissolved())
    {
      if (checkDissolveRoom(table))
      {
        char buf[100];
        sprintf(buf, "Will dissolve %s\n", table->getTitle().c_str());
        Sys_LogTournament(buf);
        
        table->setTournamentState(CTable::TS_Dissolved);
        dissolveTable(table);
        rc = true;
      }
      else
      {
        char buf[100];
        sprintf(buf, "No room to dissolve %s\n", table->getTitle().c_str());
        Sys_LogTournament(buf);
      }
    }
    else
    {
      printf("Table being dissolved\n");
    }
  }
  
  return rc;
}


void CRunState::dissolveTable(CTable* table)
{
  subState_ = RS_Dissolve;
  table_ = table;
  
  // Send dissolve query pdu to table,
  // when reply arrives, do the dissolve
  CpduDissolveTable pdu2;
  pdu2.sendTo(table->getSocket());
  
  // Pause all tables for a moment
  CpduSynchronize pdu1;
  pdu1.sendSync(tournament_->getTournamentQueue(), SF_Pause);
}

// Use type name shorthands
typedef list<CTimedEvent*> TimedEvents;
typedef list<CPlayerEvent*> PlayerEvents;
typedef list<CTableEvent*> TableEvents;

void CRunState::run(long now)
{
    printf("CRunState::run %i\n", now);

    now_ = now;

    if (lastAction_ == 0)
        lastAction_ = now;
    if (startTime_ == 0)
        startTime_ = now;

    if (now - lastAction_ > 20 &&
        subState_ == RS_Running && checkDissolve())
    {
        lastAction_ = now;
        subState_ = RS_Dissolve;
        return;
    }

    CBaseTournamentQueue* tq =
      tournament_->getTournamentQueue();

    // Get the head event of each container, check
    // for condition and execute it if condition is
    // true.
    TimedEvents& timedEvents = tournament_->timedEvents();
    PlayerEvents& playerEvents = tournament_->playerEvents();
    TableEvents& tableEvents = tournament_->tableEvents();

    if (timedEvents.size() > 0)
    {
        CTimedEvent* event = *timedEvents.begin();
        if (event->test(tournament_, now))
        {
            timedEvents.erase(timedEvents.begin());
            delete event;
        }
    }

    if (playerEvents.size() > 0)
    {
        CPlayerEvent* event = *playerEvents.begin();
        if (event->test(tournament_, tq->countPlayers()))
        {
            playerEvents.erase(playerEvents.begin());
            delete event;
        }
    }

    if (tableEvents.size() > 0)
    {
        CTableEvent* event = *tableEvents.begin();
        if (event->test(tournament_, tq->getNumTables()))
        {
            tableEvents.erase(tableEvents.begin());
            delete event;
        }
    }

    if (checkEndCondition())
    {
        setNextState(new CEndState(tournament_));
        delete this;
    }
}


void CRunState::dissolve(long now)
{
    printf("CRunState::dissolve %i\n", now);

    long dt = now - lastAction_;
    if (dt > DISSOLVE_TIME)
    {
        subState_ = RS_Running;

        // Don't run the tournament if it was paused for
        // certain time - otherwise the break would be cut short
        if (!tournament_->isPaused())
        {
            CpduSynchronize pdu;
            pdu.sendSync(tournament_->getTournamentQueue(),
                         SF_Play);
        }
    }
}


void CRunState::playerReseat(CTable*     newTable,
                             const char* username)
{
    dumpTables();
}


bool CRunState::checkDissolveRoom(CTable* dissolveTable)
{
    bool rc = false;

    int numPlayers = dissolveTable->getNumPlayersSeated();

    CBaseTournamentQueue* tq =
      tournament_->getTournamentQueue();
    TableList& tables = tq->getTableList();

    for (TableList::const_iterator tb = tables.begin(),
                                   te = tables.end();
         tb != te && numPlayers > 0;
         ++tb)
    {
        CTable* table = (*tb).get();
        if (table != dissolveTable)
        {
            numPlayers -= (tq->getTableMax() - table->getNumPlayersSeated());
        }
    }

    return numPlayers <= 0;
}


bool CRunState::canAddPlayer(const CPlayer& p)
{
    return CTournamentState::canAddPlayer(p);
}

bool CRunState::allowRelogin() const
{
  // Allow relogin only if a table is not
  // being dissolved currently.
  // XXX bug: user sees an incorrect message if subState_ is
  // RS_Dissolve. Still, this is better than to let the seating
  // to get stuck in an infinite loop.
  if (subState_ == RS_Dissolve)
    return false;
  else
    return true;
}

bool CRunState::isDissolving() const
{
  CBaseTournamentQueue* tq =
    tournament_->getTournamentQueue();
  TableList& tables = tq->getTableList();
  
  for (TableList::const_iterator tb = tables.begin(),
                                 te = tables.end();
       tb != te;
       ++tb)
  {
    CTable* table = (*tb).get();
    if (table->isDissolved())
      return true;
  }
  
  return false;
}