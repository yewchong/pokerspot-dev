#ifndef __lounge_lounge_h__
#define __lounge_lounge_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CLounge
  Created  : 04/16/2000

  OVERVIEW : This class manages the CLounge data:
             - players
             - tables
             - waiting lists

 ****************************************************************************/

#include "def.h"
#include "compat.h"
#include <map>
#include <list>
#include "lounge/playeriterator.h"
#include "lounge/tableiterator.h"
#include "lounge/loginplayer.h"
#include "lounge/waitinglist.h"
#include "loadbalancer/playerproxy.h"
#include "loadbalancer/tableproxy.h"

class CTableMaster;
class CdbHandler;
class CdbInterface;

class CLounge
{
public:
    static CLounge* Inst();
    CLounge();
    ~CLounge();

    void setDbase(CdbHandler*);
    void setShutdown(u_int16_t shutdown);
    u_int16_t getShutdown() const;
    long getMaxPlayers() const;

    void tick(long now);
    void processLogins(long now);
    void processLogouts(long now);
    void spawnTable(u_int16_t gameType,
                    u_int16_t queueIndex, 
                    u_int16_t tableMax,
                    const CChips& tableLow,
                    const CChips& tableHigh,
                    bool      hiloSplit,
		                bool      persistent = false);

    //
    // Tables and waiting lists management
    //
    bool addTableMaster(u_long addr,
                        u_short port);
    bool addWaitingList(int number,
                        u_int16_t tabletype,
                        u_int16_t queuemax,
                        u_int16_t tablemax,
                        u_int16_t tablelow,
                        u_int16_t tablehigh,
                        int spawnthreshold,
                        int initspawn);

    //
    // Financials
    //
    bool doDebit(CPlayer *, u_int16_t amount, u_int16_t index);
    bool doCredit(CPlayer *, u_int16_t amount);
    char *checkTransaction(CPlayer *);

    //
    // Player log in / out
    //
    bool login(SOCKET sd,
               const char* username,
               const char* password,
               u_int16_t   platform,
               u_byte_t* checksum);
    void postProcessLogins();
    bool checkVersion(SOCKET sd,
                      u_int16_t platform,
                      u_byte_t* checksum,
                      LoginIT& pos);
    bool validate(SOCKET sd,
                  const char* username,
                  const char* password,
                  LoginIT& pos);
    bool logout(SOCKET sd,
                CPlayer* player,
                bool force = false);
    bool logoutCertain(SOCKET sd,
                       CPlayer* player);
    bool addPlayer(SOCKET, struct in_addr*);
    bool removePlayer(CPlayer*, bool force = false);

    CPlayer* addRemotePlayer(const char* username);
    bool removeRemotePlayer(const char* username);

    //
    // Add / remove players to/from waiting lists
    //
    bool queuePlayer(CPlayer*, u_int16_t queue);
    bool cancelQueue(CPlayer*, u_int16_t queue);
    CWaitingList* findWaitingList(u_int16_t queueNumber);
    CWaitingList* findWaitingListWithTable(u_int16_t tableNumber);

    PlayerIterator allPlayers();
    PlayerIterator players();
    int numPlayers() const;
    CPlayer* findPlayer(const char* username, bool searchRemote = false);

    //
    // Table management
    //
    bool addTable(SOCKET, u_long addr);
    void addTableToWaitingList(CTable* table,
                               u_int16_t gameType);
    bool tableLogout(SOCKET sd, CTable* table);

    void addRemoteTable   (int queueNumber,
                           int tableNumber,
                           u_long ipaddress,
                           int port);
    void removeRemoteTable(int queueNumber,
                           int tableNumber);


    CTable* findTable(u_int16_t tableNumber);
    TableIterator tables();
    TableIteratorC tables() const;
    int numTables() const;

    void playerSeated(const char* username,
                      CTable*     table);
    void playerUnseated(const char* username,
                        CTable*     table);

    void dumpConfig();

    int getNumber() const;
    int getLoungeID() const;
    void setNumber(int);

    enum NotifyReason
    {
      AddPlayer        = 1,
      RemovePlayer     = 2,
      AddTable         = 3,
      RemoveTable      = 4,
      AddTournament    = 5,
      RemoveTournament = 6
    };

    void notifyOthers(NotifyReason, const char* username);
    void notifyOthers(NotifyReason, int number);

    bool queuePlayerLogic(int queueNumber, const char* username);
    bool queuePlayerLogic(CWaitingList*, CPlayer*);
    bool cancelQueuePlayerLogic(int queueNumber, const char* username);
    bool cancelQueuePlayerLogic(CWaitingList*, CPlayer*);

private:
    bool removeTable(CTable*);
    void removeTableFromWaitingList(CTable* table);

    int number_;

    // ptr to the database interface
    CdbInterface* dbase_;

    // Logged in players:
    Players players_;
    // Players logging in are kept in separate list:
    list<CLoginPlayer> loggingIn_;
    list<CLoginPlayer> loginComplete_;
    // Players logging out are kept in separate list
    list<PlayerPtr> loggingOut_;

    // Running tables:
    Tables  tables_;
    // Tables logging out are kept in separate list
    list<TablePtr> loggingOutTables_;

    static CLounge* pInstance_;

    u_int16_t shutdown_;


    // Remote players (i.e., players logged in to other
    // lounges)
    RemotePlayers remotePlayers_;

    // Remote tables (i.e., tables running in other lounges
    RemoteTables remoteTables_;


    void collectGarbage();
    long lastGarbage_;
};


inline CLounge* CLounge::Inst() 
{
    return pInstance_;
}

inline int CLounge::numPlayers() const
{   // Logged in players
    return players_.size();
}

inline int CLounge::numTables() const
{   
    return tables_.size();
}

inline void CLounge::setShutdown(u_int16_t shutdown)
{
    shutdown_ = shutdown;
}

inline u_int16_t CLounge::getShutdown() const
{
    return shutdown_;
}

inline int CLounge::getNumber() const
{
  return number_;
}

inline int CLounge::getLoungeID() const
{
  return number_;
}

inline void CLounge::setNumber(int number)
{
  number_ = number;
}


extern void tolower(string&);

#endif
