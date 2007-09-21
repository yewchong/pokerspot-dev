/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitingList
  Created  : 04/16/2000

  OVERVIEW : This struct maintains information about a
             waiting list.

             There is one waiting list per table type.

 ****************************************************************************/

#include "def.h"
#include "lounge/waitinglist.h"
#include "lounge/queueservice.h"
#include "lounge/lounge.h"
#include "lounge/game.h"
#include "pdu/pduplayerloginquery.h"
#include "pdu/pduqueueupdate.h"
#include "pdu/pdusitting.h"
#include "pdu/pduqueuefull.h"
#include "dbhandler/dbinterface.h"
#include "common/chips.h"
#include "sys.h"
#include <stdio.h>
#include <algorithm>

int GetTableSpawnThreshold(size_t numTables);


CWaitingList::CWaitingList(CGame*    pGame,
                           int       queueNumber,
                           u_int16_t gameType,
                           u_int16_t queueMax,
                           u_int16_t tableMax,
                           const CChips& lo,
                           const CChips& hi,
                           int spawnThreshold,
                           int maxTables)
  :
  pGame_      (pGame),
  queueNumber_(queueNumber),
  gameType_   (gameType),
  queueMax_   (queueMax),
  tableMax_   (tableMax),
  lolimit_    (lo),
  hilimit_    (hi),
  maxTables_  (maxTables),
  spawnThreshold_(spawnThreshold),
  state_      (WS_Servicing),
  spawnTime_  (0),
  isOwner_    (true)
{}

CWaitingList::CWaitingList(const CWaitingList& rhs)
  :
  pGame_   (rhs.pGame_),
  queueNumber_(rhs.queueNumber_),
  gameType_(rhs.gameType_),
  queueMax_(rhs.queueMax_),
  tableMax_(rhs.tableMax_),
  lolimit_ (rhs.lolimit_),
  hilimit_ (rhs.hilimit_),
  maxTables_(rhs.maxTables_),
  spawnThreshold_(rhs.spawnThreshold_),
  state_   (rhs.state_),
  spawnTime_(0),
  players_ (rhs.players_),
  tables_  (rhs.tables_),
  isOwner_ (rhs.isOwner_)
{}

CWaitingList& CWaitingList::operator = (const CWaitingList& rhs)
{
  if (this != &rhs)
  {
    pGame_ = rhs.pGame_;
    gameType_ = rhs.gameType_;
    queueNumber_ = rhs.queueNumber_;
    queueMax_ = rhs.queueMax_;
    tableMax_ = rhs.tableMax_;
    lolimit_  = rhs.lolimit_;
    hilimit_  = rhs.hilimit_;
    maxTables_ = rhs.maxTables_;
    spawnThreshold_ = rhs.spawnThreshold_;
    players_  = rhs.players_;
    tables_   = rhs.tables_;
    state_    = rhs.state_;
    spawnTime_ = rhs.spawnTime_;
    isOwner_ = rhs.isOwner_;
  }
  return *this;
}

CWaitingList::~CWaitingList()
{}


// The player can be added to queue if he has
// 10 * lolimit chips.
bool CWaitingList::canAddPlayer(CPlayer& player)
{
  CChips chips;
  CdbInterface::Inst()->getChips(&player, chips);

  if ((10 * getLo()) > chips)
  {
    CpduQueueFull pdu;
    pdu.sendQueueFull(getQueueNumber(),
                      &player,
                      QF_NotEnoughChips);
    return false;
  }

  return true;
}

bool CWaitingList::addPlayer(CPlayer& player)
{
  if (players_.size() >= queueMax_)
    return false; // queue is full

  bool rc = false;

  PlayerPtr ptr(player);
  // Player must not be in the list already.
  // Check is based on username - uniqueness assumed!
  if (find(players_.begin(), players_.end(), ptr) == players_.end())
  {   // okay he's not there - add
    players_.push_back(QueueEntry(player));
    rc = true;
    
    // Try seating him immediately if possible
    service(players_.back());
    
    // prepare this waiting list to be serviced
    CQueueService::Schedule(this);
  }
  
  return rc;
}

bool CWaitingList::removePlayer(CPlayer& player)
{
  bool rc = false;
  
  PlayerList::iterator it = find(players_.begin(),
                                 players_.end(),
                                 PlayerPtr(player));
  
  if (it != players_.end())
  {
    players_.erase(it);
    rc = true;
    
    if (players_.size() == 0)
    {   // queue size is 0 - no need to schedule
      // this queue any more
      CQueueService::StopSchedule(this);
    }
  }
  
  return rc;
}

//
// If players are to be serviced, then
// - if there is room in tables, send player there
// - if no room, decide if we should spawn table
//
int CWaitingList::tick(long now)
{
  if (!isOwner())
  {
    // This waiting list is owned by another Lounge
    // Server instance
    return 0;
  }

  if (CLounge::Inst()->getShutdown())
  {
    printf("Server is shutting down, waiting list not serviced.\n");
    return 0;
  }
  
  printf("   Waiting list %d has %d players waiting.\n",
         queueNumber_, players_.size());
  
  const int TimeBetweenSpawns = 60;
  
  if (state_ == WS_Spawning &&
      (now - spawnTime_ > TimeBetweenSpawns))
  { // more than N seconds has passed since
    // last spawn
    state_ = WS_Servicing;
  }
  
  if (state_ == WS_Servicing)
  {
    // If we have more players in queue than the
    // spawn threshold, spawn new table
    if (players_.size() >= GetTableSpawnThreshold(tables_.size()) )
    {
      state_ = WS_Spawning;
      CLounge::Inst()->spawnTable(pGame_->getGameType(),
                                  getGameType(), // queueIndex!
                                  getTableMax(),
                                  getLo(),
                                  getHi(),
                                  pGame_->isHiLoSplit());
      spawnTime_ = now;
    } 
  }
  
  return 0;
}

string CWaitingList::getTitle() const
{
  char buf[128];
  sprintf(buf, "%d/%d", lolimit_, hilimit_);
  return buf;
}


bool CWaitingList::addTable(u_int16_t gameType, CTable* table)
{
  bool rc = false;

  // As additional checks, make sure the table is
  // not in the list already
  TableList& tlist = getTableList();
  TableList::iterator tpos = find_if(tlist.begin(),
                                     tlist.end(),
                                     same_socket(table->getSocket()));
  if (tpos == tlist.end())
  {
      tpos = find_if(tlist.begin(),
                     tlist.end(),
                     same_number(table->getNumber()));
  }

  if (tpos == tlist.end())
  {   // okay its not in the list, add it
      tlist.push_back(TablePtr(*table));
      rc = true;
  }
  else
  {
    char s[128];
    sprintf(s, "CLounge: Table is already in waiting list: type %d hi: %d lo: %d",
            gameType, table->getLo(), table->getHi());
    Sys_LogError(s);
  }

  return rc;
}

// 
// This mfunction is called when a table has room
// for new players.
// 
// The seating procedure is to send CpduPlayerLoginQuery
// to the table for each player to be seated. Upon receiving
// acknowledgement, the player is seated to that table.
//
void CWaitingList::seatPlayers(CTable* table)
{
  int canSeat = getTableMax() - table->getNumPlayers();

  // Go through players in queue and initiate seating procedure
  for (PlayerList::iterator i = players_.begin(),
                            e = players_.end();
       (canSeat > 0) && (i != e);
       ++i)
  {
    QueueEntry& qe = *i;
    if (qe.state_ == QE_Waiting)
    {
      qe.state_ = QE_BeingSeated;
      canSeat--;
      CpduPlayerLoginQuery pdu;
      pdu.sendLoginQuery(table, qe.player_.get());
    }
  }
}

// 
// Seating was cancelled for this player.
// The only (proper) reason for cancellation is
// that the player is already playing at that
// table.
// 
// Try giving the cancelled seat to the next player
// in queue.
//
void CWaitingList::cancelSeat(CPlayer* player, CTable* table)
{
  PlayerPtr ptr(*player);

  PlayerList::iterator pos = find(players_.begin(),
                                  players_.end(),
                                  ptr);
  if (pos != players_.end())
  {
    (*pos).state_ = QE_Waiting;

    ++pos;
    while (pos != players_.end())
    {
      QueueEntry& qe = (*pos);
      if (qe.state_ == QE_Waiting)
      { // Try seating this player
        qe.state_ = QE_BeingSeated;
        CpduPlayerLoginQuery pdu;
        pdu.sendLoginQuery(table, qe.player_.get());
        pos = players_.end();
      }
      else
      {   // Try next player
        ++pos;
      }
    }
  }
}

//
// New player joins a queue. Try seating him.
//
// If the queue is empty, or all players before
// the new player are being seated, then try seating
// this one as well.
//
void CWaitingList::service(QueueEntry& qe)
{
/* For now, we try seating the player immediately
   even if there are other players in the queue
    if (players_.size() > 1)
    {
        // XXX check if other players in front of
        // new player are being seated - if so try
        // this one as well!
        return; 
    }
*/

    // Try seating the player to first table
    // that has room
    for (TableList::iterator ti = tables_.begin(),
                             te = tables_.end();
         ti != te;
         ++ti)
    {
        CTable* table = (*ti).table_;
        if (table->getNumPlayers() < getTableMax())
        {
           // Send login query for this table
           qe.state_ = QE_BeingSeated;
           CpduPlayerLoginQuery pdu;
           pdu.sendLoginQuery(table, qe.player_.get());
           break;
        }
    }    
}

//
// Table accepted the player - seat him.
//
void CWaitingList::seatPlayer(CPlayer* player, CTable* table)
{
  // Remove player from waiting list
  bool rc = removePlayer(*player);
  
  // broadcast queue update
  if (rc)
  {
    CpduQueueUpdate pdu;
    pdu.sendQueueServiced(getQueueNumber(), player);
  }
  
  CpduSitting pdu;
  pdu.sendSitting(player,
                  table->getNumber(),
                  table->getPort(),
                  table->getAddr());
}


int CWaitingList::getTableQueueUpdate(int length, char*& buf)
{
    pdu_queuedelta data;
    memset(&data, 0, sizeof(data));

    int numDeltas = 0;
    TableList& tableList = getTableList();

    for (TableList::iterator ti = tableList.begin(),
                             te = tableList.end();
         ti != te;
         ++ti)
    {
        if (length + (numDeltas + 1) * sizeof(pdu_queuedelta) >= MAXBUFFERSIZE)
        {
            Sys_LogError("CpduQueueUpdate::sendAllTables: buffer size too small!\n");
            break;
        }

        numDeltas++;
        data.queueNumber = htons(getQueueNumber());
        data.reason = htons(CpduQueueUpdate::QUEUEDELTA_TABLE_ADD);
        strncpy(data.name, (*ti).table_->getTitle().c_str(), PDU_STRINGSIZE);
        memcpy(buf, &data, sizeof(data));
        buf += sizeof(data);
    }

    return numDeltas;
}


void CWaitingList::tableUpdate(CTable* table,
                               int oldNumPlayers)
{
    if (oldNumPlayers > table->getNumPlayers())
    {
        // Table has new room, try seating players in queue
        seatPlayers(table);
    }
}


int GetTableSpawnThreshold(size_t numTables)
{
  switch (numTables)
  {
  case 0:
    return 2;

  case 1:
    return 4;

  case 2:
    return 5;

  case 3:
    return 6;

  case 4:
    return 7;

  case 5:
  default:
    return 8;
  }
}


bool CWaitingList::removeTable(int tableNumber)
{
  bool rc = false;

  TableList& tlist = getTableList();
  TableList::iterator tpos = find_if(tlist.begin(),
                                     tlist.end(),
                                     same_number(tableNumber));
  if (tpos != tlist.end())
  {
    rc = true;
    tlist.erase(tpos);
  }

  return rc;
}
