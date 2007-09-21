#ifndef __lounge_basetournamentqueue_h__
#define __lounge_basetournamentqueue_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CBaseTournamentQueue
  Created  : 07/07/2000

  OVERVIEW : This base class defines the basic tournament queue
             behavior. CTournamentQueue and CSatelliteQueue derive
             from this class.

 ****************************************************************************/

#include "lounge/waitinglist.h"

class CChips;
class CTournament;

class CBaseTournamentQueue : public CWaitingList
{
public:
    static CBaseTournamentQueue* FindQueue(CTable*);
    static CBaseTournamentQueue* FindQueueByTournamentNumber(u_int16_t);
    static CBaseTournamentQueue* FindQueueByIndex(u_int16_t);

    virtual void redefineThis() const = 0;

    CBaseTournamentQueue(CGame*    pGame,
                         int       number,
                         u_int16_t gameType,
                         u_int16_t queueMax,
                         u_int16_t tableMax,
                         const CChips& lo,
                         const CChips& hi,
                         int spawnThreshold,
                         int maxTables = MaxTablesDefault);
    virtual ~CBaseTournamentQueue();

    void addTournament(int number,
                       int maxPlayers,
                       const CChips& buyin,
                       const CChips& startChips,
                       const CChips& startLow,
                       u_int32_t startTime,
                       const string& descr,
                       const string& script);
    CTournament* findTournament(int number);
    CTournament* getTournament();
    CTournament* findTournamentByTable(int number);
    void removeTournament();

    int countPlayers();
    int getNumTables() const;
    CTable* findTableWithLeastPlayers();
    CTable* getRandomTable(CTable* notThis);
    CTable* findTableWithPlayer(const char* username);

    void playerUnseated(const char* username, CTable*);
    bool isTournamentTable(CTable*) const;
    bool tableDissolveInProgress(CTable*) const;

    //
    // CWaitingList overrides
    //
    virtual int tick(long now);
    virtual bool canAddPlayer(CPlayer&);
    virtual bool addPlayer(CPlayer&);

    int getTableQueueUpdate(int length, char*& buf);
    bool addTable(u_int16_t gameType, CTable*);
    void tableUpdate(CTable*, int);
    void seatPlayers(CTable*);
    bool removePlayer(CPlayer&);
    void seatPlayer(CPlayer*, CTable*);

protected:
    CTournament* tournament_;
};

inline int CBaseTournamentQueue::getNumTables() const
{
    return tables_.size(); 
}

#endif
