/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CBaseTournamentQueue
  Created  : 07/07/2000

  OVERVIEW : This is a special kind of a waiting list that
             manages tournaments.

 ****************************************************************************/

#include "tournament/parser.h"
#include "tournament/basetournamentqueue.h"
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
#include "sys.h"

#ifdef _WIN32
#pragma warning( disable : 4786 )
#endif

// This list is used to access the tournament queue instances
static list<CBaseTournamentQueue*> tourneys;

CBaseTournamentQueue* FindTournament(CBaseTournamentQueue* tq)
{
  for (list<CBaseTournamentQueue*>::iterator b = tourneys.begin(),
                                             e = tourneys.end();
       b != e;
       ++b)
  {
    if (*b == tq)
    {
      return tq;
    }
  }
  return NULL;
}

void AddTournament(CBaseTournamentQueue* tq)
{
  if (FindTournament(tq) == NULL)
  {
    tourneys.push_back(tq);
  }
}

void RemoveTournament(CBaseTournamentQueue* tq)
{
  for (list<CBaseTournamentQueue*>::iterator b = tourneys.begin(),
                                             e = tourneys.end();
       b != e;
       ++b)
  {
    if (*b == tq)
    {
      tourneys.erase(b);
      break;
    }
  }
}


CBaseTournamentQueue::CBaseTournamentQueue(
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
    CWaitingList(pGame, number, gameType,
                 queueMax, tableMax, lo, hi,
                 spawnThreshold, maxTables),
    tournament_(NULL)
{
  AddTournament(this);
}

CBaseTournamentQueue::~CBaseTournamentQueue()
{
  delete tournament_;
  tournament_ = NULL;
}

int CBaseTournamentQueue::tick(long now)
{
  if (!isOwner())
  {
    // This waiting list is owned by another Lounge
    // Server instance
    return 0;
  }

  if (tournament_)
    tournament_->tick(now);

  return 0;
}


void CBaseTournamentQueue::addTournament(
                                     int number,
                                     int maxPlayers,
                                     const CChips& buyin,
                                     const CChips& startChips,
                                     const CChips& startLow,
                                     u_int32_t startTime,
                                     const string& descr,
                                     const string& script)
{
    if (!tournament_)
    {
        tournament_ = new CTournament(this,
                                      number,
                                      maxPlayers,
                                      buyin,
                                      startChips,
                                      startLow,
                                      startTime,
                                      descr,
                                      script);

        CParser parser(tournament_);
        parser.loadScript(script.c_str());

        CpduQueueUpdate pdu;
        pdu.sendQueueAdd(getQueueNumber(), tournament_);

        CQueueService::Schedule(this);
    }
    else
    {
        if (!tournament_->isRunning())
        {
            bool update = false;
            if (tournament_->getNumber() != number)
            {
                update = true;
                CpduQueueUpdate pdu;
                pdu.sendQueueRemove(getQueueNumber(), tournament_);
            }

            tournament_->setNumber(number);
            tournament_->setBuyin(buyin);
            tournament_->setStartTime(startTime);
            tournament_->setDescription(descr);

            CParser parser(tournament_);
            parser.loadScript(script.c_str());

            if (update)
            {
                CpduQueueUpdate pdu;
                pdu.sendQueueAdd(getQueueNumber(), tournament_);
            }
        }
    }
}


void CBaseTournamentQueue::removeTournament()
{
    if (tournament_ && !tournament_->isRunning())
    {
      CpduQueueUpdate pdu;
      pdu.sendQueueRemove(getQueueNumber(), tournament_);

      delete tournament_;
      tournament_ = NULL;
    }
}


CTournament* CBaseTournamentQueue::findTournament(int number)
{
    if (tournament_ && tournament_->getNumber() == number)
      return tournament_;
    else
      return NULL;
}


CTournament* CBaseTournamentQueue::getTournament()
{
    return tournament_;
}


CTournament* CBaseTournamentQueue::findTournamentByTable(int number)
{
    // Check that the table is of our queue!
    for (TableList::iterator tb = tables_.begin(),
                             te = tables_.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        if (table->getNumber() == number)
            return tournament_;
    }

    return NULL;
}


int CBaseTournamentQueue::getTableQueueUpdate(int length, char*& buf)
{
    int numDeltas = CWaitingList::getTableQueueUpdate(length, buf);

    if (tournament_)
    {
        pdu_queuedelta data;
        memset(&data, 0, sizeof(data));

        if (length + (numDeltas + 1) * sizeof(pdu_queuedelta) < MAXBUFFERSIZE)
        {
            numDeltas++;
            data.queueNumber = htons(getQueueNumber());
            data.reason = htons(CpduQueueUpdate::QUEUEDELTA_TABLE_ADD);
            strncpy(data.name, tournament_->getTitle().c_str(), PDU_STRINGSIZE);
            memcpy(buf, &data, sizeof(data));
            buf += sizeof(data);
        }
        else
        {
            Sys_LogError("CpduQueueUpdate::sendAllTables: buffer size too small!\n");
        }
    }

    return numDeltas;
}

bool CBaseTournamentQueue::addTable(u_int16_t gameType, CTable* table)
{
    bool rc = CWaitingList::addTable(gameType, table);

    if (rc)
    {
        CTournament* t = tournament_;
        ASSERT(t);
        if (!t) return false;

        // Set initial game type & limits
        CpduTournamentParams pdu;
        pdu.sendTournamentParams(table,
                                 TF_Number,
                                 t->getNumber(),
                                 t->getNumPlayers());
        pdu.sendTournamentParams(table,
                                 TF_GameType,
                                 t->getGameType(),
                                 t->isHiLo() ? 1 : 0);
        pdu.sendTournamentParams(table,
                                 TF_Limit,
                                 t->getStartLo().getRep(),
                                 (2 * t->getStartLo()).getRep());
    }

    return rc;
}


void CBaseTournamentQueue::tableUpdate(CTable* table,
                                       int oldNumPlayers)
{
  // Tournament state handles table dissolves when needed.
}

// Maximum number of players in tournament queue.
// Two additional tables will be spawned to allow late
// comers to join too, so the total number of players is
// MaxPlayerInTournament + 20.
const int MaxPlayersInTournament = 100;

bool CBaseTournamentQueue::canAddPlayer(CPlayer& p)
{
  // Do we have a tournament scheduled?
    if (tournament_ == NULL)
    {
        CpduQueueFull pdu;
        pdu.sendQueueFull(getQueueNumber(),
                          &p,
                          QF_NotValidForTournament+1);
        CpduNotify pdu2;
        pdu2.sendNotifyMessage(p.getSocket(), 
                               "There is no tournament currently scheduled.",
                               NF_NoCloseConnection);

        return false;
    }

    int n = numPlayers();
    if (n >= MaxPlayersInTournament)
    {
        CpduQueueFull pdu;
        pdu.sendQueueFull(getQueueNumber(),
                          &p,
                          QF_QueueFull);
        return false;
    }

    // Check if the player is allowed to play in
    // the tournament.
    if (!CdbInterface::Inst()->checkTournament(p))
    {
        CpduQueueFull pdu;
        pdu.sendQueueFull(getQueueNumber(),
                          &p,
                          QF_NotValidForTournament+1);
        CpduNotify pdu2;
        pdu2.sendNotifyMessage(p.getSocket(), 
                               "You are not qualified to play in this tournament.",
                               NF_NoCloseConnection);

        return false;
    }

    // Player is allowed to login back if he's playing
    // at a table as a zombie
    if (tournament_ && tournament_->allowRelogin())
    {

        // First check if the player is already sitting
        // at one of the tables - if so, it might be a
        // zombie and the player should be allowed to be
        // reseated at the table.

        CTable* table = findTableWithPlayer(p.getUsername());
        if (table)
        {
            CpduSeatPlayer pdu;
            pdu.sendSeatPlayer(p.getUsername(),
                               0, // Zombie has chips already
                               SF_SendAck|SF_CheckZombie, // seat on ack
                               table);   

            // Return FALSE so the queue update
            // won't be sent!
            return false;                        
        }
    }

    bool rc = true;

    if (tournament_)
    {
        rc = tournament_->canAddPlayer(p);
    }

    if (rc)
    {
        if (getLo() > 0)
        {
            CChips chips;
            CdbInterface::Inst()->getChips(&p, chips);

            if (getLo() > chips)
            {
                CpduQueueFull pdu;
                pdu.sendQueueFull(getQueueNumber(),
                                  &p,
                                  QF_NotEnoughChips);
                return false;
            }
        }
    }
    
    return rc;
}

bool CBaseTournamentQueue::addPlayer(CPlayer& p)
{
    bool rc = false;

    if (tournament_ &&
        tournament_->isUnfrozen())
    {
        if (players_.size() >= queueMax_)
            return false; // queue is full

        // When tournament is running unfrozen,
        // seat the new player immediately. He
        // will get a "Free Seat".

        //CTable* table = getRandomTable(NULL);
        // first come, first served for now!
        CTable* table = NULL;
        if (!table)
        {   // no luck, find first table that has room

            for (TableList::iterator ti = tables_.begin(),
                                     te = tables_.end();
                ti != te;
                ++ti)
            {
                CTable* t = (*ti).table_;
                if (t->getNumPlayersSeated() < getTableMax())
                {
                    table = t;
                    break;
                }
            }
        }    

        if (table)
        {
            if (CdbInterface::Inst()->buyin(&p, getLo()))
            {
              CpduSeatPlayer pdu;
              pdu.sendSeatPlayer(p.getUsername(),
                                 0, // Free Seat has chips already
                                 0, // actual seating takes place immediately, not on ack                                      
                                table);

              CLounge::Inst()->playerSeated(p.getUsername(), table);
              seatPlayer(&p, table);

              tournament_->setNumPlayers(countPlayers());

              rc = true;
            }
            else
            {
              char buf[200];
              sprintf(buf, "$%f", getLo().asDouble());

              string s;
              s = string("Unable to perform ") + string(buf) + string(" buy-in - tournament participation cancelled.");
              CpduNotify pdu;
              pdu.sendNotifyMessage(p.getSocket(),
                                    s.c_str(), NF_NoCloseConnection);

              s = string("Player ") + p.getUsername() + string(" tournament buyin failed\n");
              Sys_LogError(s.c_str());
            }
        }
        else
        {   // all tables filled, could not add!
            rc = false;
        }
    }
    else
        rc = CWaitingList::addPlayer(p);

    return rc;
}


void CBaseTournamentQueue::seatPlayers(CTable*)
{
    // Override to do nothing - the seating
    // is done differently in tournament
}


// Override to not to stop scheduling.
bool CBaseTournamentQueue::removePlayer(CPlayer& player)
{
    bool rc = false;

    PlayerList::iterator it = find(players_.begin(),
                                   players_.end(),
                                   PlayerPtr(player));

    if (it != players_.end())
    {
        players_.erase(it);
        rc = true;
    }

    return rc;
}


int CBaseTournamentQueue::countPlayers()
{
    int numPlayers = 0;

    for (TableList::iterator tb = tables_.begin(),
                             te = tables_.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        numPlayers += table->getNumPlayersSeated();
    }

    return numPlayers;
}


void CBaseTournamentQueue::playerUnseated(const char* username, CTable* table)
{ 
    if (!isTournamentTable(table)) return;

    if (table->getNumPlayersSeated() == 0)
    {
        CpduTournamentParams pdu;
        pdu.sendTournamentParams(table,
                                 TF_End, 0, 0);
    }

    char buf[100];
    sprintf(buf, "%s player %s unseated",
            table->getTitle().c_str(),
            username);
    Sys_LogTournament(buf);

    // These should match:
    int numPlayers = countPlayers();

    int joined = 0, left = 0;
/*
    CdbInterface::Inst()->countTournamentPlayers(tournament_->getNumber(),
                                                 joined,
                                                 left);
*/
    printf("*** numplayers %i diff %i joined %i left %i\n",
           numPlayers, joined - left, joined, left);

    // IF there is only one player left,
    // we have the winner
    if (numPlayers == 1)
    {
        for (TableList::iterator tb = tables_.begin(),
                                 te = tables_.end();
            tb != te;
            ++tb)
        {
            CTable* table = (*tb).get();
            list<string>& players = table->getPlayers();
            if (players.size() == 1)
            {
                string winner = (*players.begin());
                tournament_->logEvent(TE_PlayerLeave, winner.c_str(), "WINNER");

                CpduTournamentParams pdu;
                pdu.sendTournamentParams(table, TF_Winner, 0, 0);
                // Let the table stay there for 1 minutes
                pdu.sendTournamentParams(table, TF_End, 0, 0);

                break;
            }
        }

        if (!CdbInterface::Inst()->checkTournament(tournament_->getNumber()))
        {
            char buf[200];
            sprintf(buf, "CHECK: Tournament %i: count check fails",
                    tournament_->getNumber());
            Sys_LogTournament(buf);
        }
    }
}


CTable* CBaseTournamentQueue::findTableWithLeastPlayers()
{
    int least = 11;
    CTable* table = NULL;

    for (TableList::iterator tb = tables_.begin(),
                             te = tables_.end();
         tb != te;
         ++tb)
    {
        CTable* t = (*tb).get();
        if (t->getNumPlayersSeated() < least)
        {
            least = t->getNumPlayersSeated();
            table = t;
        }
    }

    return table;
}

// Pick a random table in which 'username' is not already
// seated.
CTable* CBaseTournamentQueue::getRandomTable(CTable* notThis)
{
    int i = rand() % (tables_.size() + 1);

    for (int c = 0; c < 10; c++)
    {

    for (TableList::iterator tb = tables_.begin(),
                             te = tables_.end();
         tb != te;
         ++tb)
    {
        if (i-- == 0)
        {
            CTable* t = (*tb).get();
            if (t != notThis)
                return t;
        }
        else
            break;
    }

    }

    return NULL;
}


void CBaseTournamentQueue::seatPlayer(CPlayer* player, CTable* table)
{
    // Let base class do the dirty work
    CWaitingList::seatPlayer(player, table);

    if (tournament_)
    {
        // Log this event
        tournament_->logEvent(TE_PlayerJoin,
                              player->getUsername(),
                              table->getTitle().c_str());
    }
    else
    {
        Sys_LogError("CBaseTournamentQueue::seatPlayer: No tournament\n");
    }
}


CTable* CBaseTournamentQueue::findTableWithPlayer(const char* username)
{
    for (TableList::iterator tb = tables_.begin(),
                             te = tables_.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        if (table->isPlayerSeated(username))
            return table;
    }

    return NULL;
}


bool CBaseTournamentQueue::isTournamentTable(CTable* checkTable) const
{
    for (TableList::const_iterator tb = tables_.begin(),
                                   te = tables_.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        if (table == checkTable)
            return true;
    }

    return false;
}


CBaseTournamentQueue* CBaseTournamentQueue::FindQueue(CTable* table)
{
  // For each queue
  //   if queue has table
  //     return queue
  for (list<CBaseTournamentQueue*>::iterator b = tourneys.begin(),
                                             e = tourneys.end();
       b != e;
       ++b)
  {
    CBaseTournamentQueue* tq = *b;

    if (tq->isTournamentTable(table))
      return tq;
  }

  return NULL;
}


CBaseTournamentQueue* CBaseTournamentQueue::FindQueueByTournamentNumber(u_int16_t n)
{
  // For each queue
  //   if queue has tournament number 'n'
  //     return queue
  for (list<CBaseTournamentQueue*>::iterator b = tourneys.begin(),
                                             e = tourneys.end();
       b != e;
       ++b)
  {
    CBaseTournamentQueue* tq = *b;

    if (tq->findTournament(n))
      return tq;
  }

  return NULL;
}


// Return the N'th tournament queue
CBaseTournamentQueue* CBaseTournamentQueue::FindQueueByIndex(u_int16_t index)
{
  --index;

  for (list<CBaseTournamentQueue*>::iterator b = tourneys.begin(),
                                             e = tourneys.end();
       b != e;
       ++b, --index)
  {
    if (index == 0)
    {
      CBaseTournamentQueue* tq = *b;
      return tq;
    }
  }

  return NULL;
}


bool CBaseTournamentQueue::tableDissolveInProgress(CTable*) const
{
  // XXX decide tournament based on the table
  return tournament_ && tournament_->tableDissolveInProgress();
}