#ifndef __lounge_waitinglist_h__
#define __lounge_waitinglist_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitingList
  Created  : 04/16/2000

  OVERVIEW : This struct maintains information about

              - players in a waiting list
              - tables running the particular game type

             Players joining the tables go through the
             waiting list. If there is room in any of the
             tables of this list, the player is sent to that
             table. Otherwise, the player is put in the waiting
             list.

             If the number of players in the waiting list
             grows above a threshold, a new table is spawned
             and the queued players are sent to that table.
             Players in a queue are serviced on first-come-first-served
             basis.

 ****************************************************************************/

#include "compat.h"
#include "ptr.h"
#include "table.h"
#include "tableiterator.h"
#include "queueentry.h"
#include "common/chips.h"
#include <list>
#include <string>



// Waiting list is a list of QueueEntries
typedef list<QueueEntry> PlayerList;
typedef list<TablePtr> TableList;

class CGame;

class CWaitingList
{
public:
    enum {
        MaxTablesDefault = 20
    };

public:
    CWaitingList(CGame*    pGame,
                 int       number,
                 u_int16_t gameType,
                 u_int16_t queueMax,
                 u_int16_t tableMax,
                 const CChips& lo,
                 const CChips& hi,
                 int spawnThreshold,
                 int maxTables = MaxTablesDefault);
    CWaitingList(const CWaitingList&);
    CWaitingList& operator = (const CWaitingList&);
    virtual ~CWaitingList();

    CGame*    getGame() const { return pGame_; }
    int       getQueueNumber() const { return queueNumber_; }
    u_int16_t getGameType() const { return gameType_; }
    u_int16_t getQueueMax() const { return queueMax_; }
    u_int16_t getTableMax() const { return tableMax_; }
    CChips    getLo() const { return lolimit_; }
    CChips    getHi() const { return hilimit_; }
    virtual string getTitle() const;
    int       getMaxTables() const { return maxTables_; }

    virtual bool canAddPlayer(CPlayer&);                              
    virtual bool addPlayer(CPlayer&);
    virtual bool removePlayer(CPlayer&);
    int numPlayers() const { return players_.size(); }
    PlayerList& getPlayers() { return players_; }

    virtual int tick(long now);
    virtual bool addTable(u_int16_t gameType, CTable*);
    virtual void tableUpdate(CTable*, int);
    virtual bool removeTable(int tableNumber);

    //
    // Waiting list service mfunctions; these drive the seating.
    //

    // Called when table has room
    virtual void seatPlayers(CTable*); 
    // New player joined queue, try seating
    virtual void service(QueueEntry&);
    // Called when seating is cancelled
    virtual void cancelSeat(CPlayer*, CTable*);
    // Seat query ok, send player to table
    virtual void seatPlayer(CPlayer*, CTable*);

    // Tables
    TableIterator getTables();
    TableList& getTableList() { return tables_; }
    virtual int getTableQueueUpdate(int length, char*& buf);

    bool isOwner() const;
    bool isProxy() const;
    void setOwner(bool);

protected:
    CGame* pGame_;
    int queueNumber_;
    u_int16_t gameType_;
    u_int16_t queueMax_; // max # of players allowed in queue
    // Limits of tables in this waiting list
    u_int16_t tableMax_;
    CChips    lolimit_;
    CChips    hilimit_;

    // Limit of tables per this waiting list
    int maxTables_;

    // How many players have to be in queue
    // before new table is spawned?
    int spawnThreshold_;

    enum WaitingList_State
    {
        WS_Servicing = 0,
        WS_Spawning  = 1
    };
    WaitingList_State state_;
    long spawnTime_;


    // Players in this waiting list
    PlayerList players_;

    // Table instances running this game
    TableList tables_;

    // A waiting list can be either owned or a
    // proxy to a remote-owned waiting list
    bool isOwner_;
};

inline bool CWaitingList::isOwner() const
{
  return isOwner_;
}

inline bool CWaitingList::isProxy() const
{
  return !isOwner_;
}

inline void CWaitingList::setOwner(bool b)
{
  isOwner_ = b;
}

inline bool operator == (const CWaitingList& lhs, const CWaitingList& rhs)
{
    return lhs.getQueueNumber() == rhs.getQueueNumber();
}

struct same_limits
{
    same_limits(const CChips& lo, const CChips& hi)
        : hi_(hi), lo_(lo)
    {}
    bool operator() (const CWaitingList& wl) const
    {
        return wl.getHi() == hi_ && wl.getLo() == lo_;
    }
    bool operator() (CWaitingList* wl) const
    {
        return wl->getHi() == hi_ && wl->getLo() == lo_;
    }
    CChips hi_, lo_;
};

struct same_number
{
    same_number(u_int16_t number)
        : number_(number)
    {}
    bool operator() (const CWaitingList& wl) const
    {
        return wl.getQueueNumber() == number_;
    }
    bool operator() (CWaitingList* wl) const
    {
        return wl->getQueueNumber() == number_;
    }
    bool operator() (const TablePtr& ptr) const
    {
        return ptr.table_->getNumber() == number_;
    }
    u_int16_t number_;
};


#endif
