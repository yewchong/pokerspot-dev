/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CLounge
  Created  : 04/16/2000

  OVERVIEW : This class is the entry point to the lounge data:
             - players
             - tables
             - waiting lists

 ****************************************************************************/

#include "lounge/lounge.h"
#include "lounge/game.h"
#include "lounge/waitinglist.h"
#include "lounge/tablemaster.h"
#include "pdu/pduloginreject.h"
#include "pdu/pdulogoutconfirmed.h"
#include "pdu/pduloginaccept.h"
#include "pdu/pduqueueupdate.h"
#include "pdu/pduqueuefull.h"
#include "pdu/pdunotify.h"
#include "pdu/pduloungestats.h"
#include "pdu/pduspawntable.h"
#include "pdu/pduupgraderequest.h"
#include "loadbalancer/loadbalancer.h"
#include "loadbalancer/pduwaitinglistevent.h"
#include "dbhandler/dbinterface.h"
#include "upgrade/autoupgrade.h"
#include "poller.h"
#include "server.h"
#include "sys.h"
#include "def.h"
#include "common/stringoutput.h"
#include <set>

// Global CLounge instance
static CLounge g_lounge;

CLounge* CLounge::pInstance_ = NULL;

// Allow same ip address log in multiple times
#ifdef _WIN32
#define NO_IPADDRESS_CHECK 1
#endif

// IP Address tracking:
// IP addresses of people currently logged in are stored
// into this set. An ip address is not allowed to connect
// more than once.
set<u_int32_t> g_IPAddresses;

// Return true if ip address already registered.
bool FindIPAddress(u_int32_t ipaddr)
{
#ifdef NO_IPADDRESS_CHECK
    return false;
#else
    return g_IPAddresses.find(ipaddr) != g_IPAddresses.end();
#endif
}

// Registers ip address, returns true if registered,
// false if ip address was already registered.
bool RegisterIPAddress(u_int32_t ipaddr)
{
#ifdef NO_IPADDRESS_CHECK
    return true;
#else
    return (g_IPAddresses.insert(ipaddr).second);
#endif
}

// Unregisters ip address, returns true if unregistered,
// false if ip address was not registered.
bool UnregisterIPAddress(u_int32_t ipaddr)
{
#ifdef NO_IPADDRESS_CHECK
    return true;
#else
    set<u_int32_t>::iterator pos = 
        g_IPAddresses.find(ipaddr);

    if (pos != g_IPAddresses.end())
    {
        g_IPAddresses.erase(pos);
        return true;
    }
    else
    {
        return false;
    }
#endif
}


void tolower(string& s)
{
  for (int i = s.size() - 1; i >= 0; i--)
    s[i] = tolower(s.at(i));
}

CLounge::CLounge()
  :
  number_     (0),
  dbase_      (0),
  shutdown_   (0),
  lastGarbage_(0)
{
  pInstance_ = this;
}

CLounge::~CLounge()
{
  delete dbase_;
  dbase_ = NULL;
  pInstance_ = NULL;
}

void CLounge::setDbase(CdbHandler* dbase)
{
  dbase_ = new CdbInterface(dbase);
}

// - Purge players logging in if they have
//   not completed login in due time.
// - Purge players logging out
void CLounge::tick(long now)
{
  processLogins(now);
  processLogouts(now);

  if (now - lastGarbage_ > 1)
  {
    lastGarbage_ = now;
    collectGarbage();
  }
}

void CLounge::processLogins(long now)
{
    // Check if somebody's been logging in too long
    // - if so, log out the player
    list<CLoginPlayer>::iterator i = loggingIn_.begin();

    while (i != loggingIn_.end())
    {
        CLoginPlayer& p = (*i);
    
        if (now - p.loginTime_ > LOGIN_TIMEOUT)
        {   
            // Login not completed withing timeout - player will
            // be forcibly logged out

            UnregisterIPAddress(p.player_.getIpAddress());

            CpduNotify pdu;
            pdu.sendNotifyMessage(p.player_.getSocket(), "Login timed out.");
            printf("Login time for socket %d exceeded - terminating connection.\n",
                   p.player_.getSocket());
            CPoller::Inst()->removePlayerSocket(p.player_.getSocket());
            Sys_CloseSocket(p.player_.getSocket());

            // XXX the client does not react very well
            // to the socket being closed like this

            i = loggingIn_.erase(i);
        }
        else
        {
            ++i;
        }
    }
}


void CLounge::processLogouts(long /*now*/)
{
    bool sendUpdate = false;

    //
    // Process logging out players
    //
    list<PlayerPtr>::iterator i = loggingOut_.begin();

    while (i != loggingOut_.end())
    {
        sendUpdate = true;

        string username = (*i).player_->name();
        SOCKET sd = (*i).player_->getSocket();

        printf("Logging out player %s socket %d.\n",
               username.c_str(), sd);

        // Forget ip address
        UnregisterIPAddress((*i).player_->getIpAddress());

        // Erase the player from the main list.
        tolower(username);

        if (players_.erase(username) == 0)
        {   // player was not in main list - erase from
            // logging in list
            LoginPlayers::iterator it = find_if(loggingIn_.begin(),
                                                loggingIn_.end(),
                                                same_socket(sd));
            if (it != loggingIn_.end())
                loggingIn_.erase(it);
        }

        // Terminate the connection
        CPoller::Inst()->removePlayerSocket(sd);
        Sys_CloseSocket(sd);

        // Notify Load Balancer and/or lounges
        notifyOthers(RemovePlayer, username.c_str());


        // Remove it from logging out list
        i = loggingOut_.erase(i);
    }

    //
    // Process logging out tables
    //
    list<TablePtr>::iterator ti = loggingOutTables_.begin();

    while (ti != loggingOutTables_.end())
    {
        sendUpdate = true;

        CTable* table = (*ti).table_;
        printf("*** Removing table %d socket %d.\n",
               table->getNumber(), table->getSocket());

        removeTable(table);

        // NOTE: after removeTable 'table' points to destroyed data

        // Remove it from logging out list
        ti = loggingOutTables_.erase(ti);
    }

    if (sendUpdate)
    {
        // Send login stats update if something was removed
        CpduLoungeStats pduStats;
        pduStats.sendStats(players_.size(), tables_.size());
    }
}

PlayerIterator CLounge::allPlayers()
{
    // The iterator goes over logging in
    // and all other players
    return PlayerIterator(loggingIn_.begin(),
                          loggingIn_.end(),
                          players_.begin(),
                          players_.end());
}

PlayerIterator CLounge::players()
{
  // The iterator goes over those players
  // who are properly logged in
  return PlayerIterator(players_.begin(),
                        players_.end());
}

TableIterator CLounge::tables()
{
    return TableIterator(tables_.begin(), tables_.end());
}

TableIteratorC CLounge::tables() const
{
    return TableIteratorC(tables_.begin(), tables_.end());
}

// Create new player instance, save it
// to logging in players list.
bool CLounge::addPlayer(SOCKET sd, struct in_addr* inaddr)
{   
    if (FindIPAddress(inaddr->s_addr))
    {
        CpduNotify pdu;
        char s[300];
        const char* saddr = inet_ntoa(*inaddr);
        if (saddr != NULL)
            sprintf(s, "Error: ip address %s is already logged in.", saddr);
        else
            sprintf(s, "Error: ip address %ld is already logged in.", inaddr->s_addr);
        pdu.sendNotifyMessage(sd, s);
        Sys_LogError(s);

        CPoller::Inst()->removePlayerSocket(sd);
        Sys_CloseSocket(sd);

        return false;
    }

    RegisterIPAddress(inaddr->s_addr);

    CLoginPlayer lp(sd, inaddr->s_addr);
    loggingIn_.push_back(lp);
    return true;
}

bool CLounge::login(SOCKET sd,
                    const char* username,
                    const char* password,
                    u_int16_t platform,
                    u_byte_t* checksum)
{
  if (shutdown_)
  {
    CpduNotify pdu;
    pdu.sendNotifyMessage(sd, "The server is shutting down - please try again later.");
    return false;
  }   
  
  // Find the player from the logging in list
  list<CLoginPlayer>::iterator pos = find_if(loggingIn_.begin(),
                                             loggingIn_.end(),
                                             same_socket(sd));
  
  // Testing: Allow 'bot_X' players to log in without login entries
  string test_string(username);
  
  if (pos == loggingIn_.end() && test_string.find("bot_") != 0)
  {
    // Player was not found from logging in list.
    // Internal logic error - should never happen
    char s[128];
    sprintf(s, "Internal error: player (%d %s, %s) not in logging in list\n",
            sd, username, password);
    Sys_LogError(s);
    CPoller::Inst()->removePlayerSocket(sd);
    Sys_CloseSocket(sd);
    return false;
  }
  
  bool rc = validate(sd, username, password, pos);
  
  if (rc)
  {      
    // login accepted - add to loginComplete_ list
    // so it'll be removed from logginIn_ list
    // and added to main player container.
    // (it's done it two steps because while we're
    // here we're iterating over the logginIn_ list)
    (*pos).username_ = username;
    (*pos).password_ = password;
    
    loginComplete_.push_back(*pos);
  }
  
  return rc;
}

void CLounge::postProcessLogins()
{
    // Removing elements from a list while iterating
    // over it is tricky. 
    //
    // Those CLoginPlayers who have completed their
    // logins are processed here by removing the
    // CLoginPlayer entries
    //
    list<CLoginPlayer>::iterator i = loginComplete_.begin(),
                                 e = loginComplete_.end();

    while (i != e)
    {
        CLoginPlayer& p = (*i);

        // Remove from logging in list
        LoginPlayers::iterator pos =
            find_if(loggingIn_.begin(),
                    loggingIn_.end(),
                    same_socket(p.player_.getSocket()));
        if (pos != loggingIn_.end())
        {
            loggingIn_.erase(pos);
        }

        // Add to main list
        // We also check that THE SAME NAME is not there already.
        // The check is case insensitive.
        u_long ipaddr = p.player_.getIpAddress();
        SOCKET sd = p.player_.getSocket();
        string checkName = p.username_;

        // Failed logins will have empty name
        if (checkName.size() > 0)
        {
            tolower(checkName);

            CPlayer player(p.username_, sd, ipaddr);
            if (players_.insert(Players::value_type(checkName.c_str(), player)).second == false)
            {   // Player with same name is logged in already!
                CpduNotify pdu;
                char s[128];
                sprintf(s, "Error: user %s is already logged in.", p.username_.c_str());
                pdu.sendNotifyMessage(sd, s);

                UnregisterIPAddress(ipaddr);
                CPoller::Inst()->removePlayerSocket(sd);
                Sys_CloseSocket(sd);
            }
            else
            {   // 
                // login completed successfully
                //
                string motd;
                dbase_->getMotdMsg(motd);
                if (motd.size() == 0)
                  motd = "Welcome to PokerSpot online cardroom!";

                // Send accept to player
                CpduLoginAccept pduAccept(p.username_.c_str(),
                                          p.password_.c_str(),
                                          motd.c_str());
                pduAccept.sendTo(sd);

                // Notify Load Balancer and/or other lounges
                notifyOthers(AddPlayer, p.username_.c_str());
            }
        }
        ++i;
    }

    loginComplete_.clear();
}

bool CLounge::validate(SOCKET sd,
                       const char* username,
                       const char* password,
                       LoginIT& pos)
{
    bool rc = dbase_->authenticate(username, password);

    if (!rc)
    {
        if ((*pos).retryCount_++ < MAX_LOGIN_RETRIES)
        {
            // Invalid username/password -
            // reject login, allow retry
            CpduLoginReject pdu;
            pdu.sendReject(sd, RF_Allow_Retry);
        }
        else
        {   // Too many login failures

            UnregisterIPAddress((*pos).player_.getIpAddress());

            // Remove the player from logging list
            loginComplete_.push_back(*pos);    
            //This code not for production:
            //loggingIn_.erase(pos);

            // Reject login, terminate connection
            CpduLoginReject pdu;
            pdu.sendReject(sd, RF_No_Retry);
            CPoller::Inst()->removePlayerSocket(sd);
            Sys_CloseSocket(sd);
        }
    }

    return rc;
}

bool CLounge::checkVersion(SOCKET sd,
                           u_int16_t platform,
                           u_byte_t* clientChecksum,
                           LoginIT& pos)
{
    // if checksum does not match, initiate auto-upgrade
    const char* configFile = AutoUpgrade::GetConfigFile(platform);
    u_byte_t* serverChecksum = AutoUpgrade::ComputeChecksum(configFile);

    if (clientChecksum == NULL || serverChecksum == NULL)
    {   // Auto-upgrade will be disabled!
        return true;
    }

    if (memcmp(clientChecksum, serverChecksum, AutoUpgrade::ChecksumSize) != 0)
    {
      //
      // Checksums differ - initiate auto-upgrade
      //
      CpduUpgradeRequest pdu;
      pdu.sendUpgradeRequest(sd, configFile);
      return false;
    }

    return true;
}

bool CLounge::logout(SOCKET sd,
                     CPlayer* player,
                     bool force)
{
    if (force)
    {   // will be logged out with no questions asked
        removePlayer(player, force);
    }
    else
    {
        // XXX
        // if player is not playing in any table,
        //   loggingOut_.push_back(PlayerPtr(player));
        // else 
        //   send CpduLogoutWarning
        removePlayer(player);
    }
    return true;
}

bool CLounge::logoutCertain(SOCKET sd,
                            CPlayer* player)
{
    removePlayer(player);
    return true;
}

bool CLounge::removePlayer(CPlayer* player, bool force)
{
    PlayerPtr ptr(*player);
    if (find(loggingOut_.begin(), loggingOut_.end(), ptr) !=
             loggingOut_.end())
    {
        return true; // already logging out
    }
    loggingOut_.push_back(ptr);

    list<pdu_queuedelta> updates;

    // Remove player from all waiting lists and
    // prepare to send queue updates to clients
    GameIterator i = CGame::GetGames();
    while (i.more())
    {
        CGame& game = i.current();
        WaitingLists& wl = game.getWaitingLists();

        for (WaitingLists::iterator wi = wl.begin(),
                                    we = wl.end();
             wi != we;
             ++wi)
        {
            CWaitingList* w = (*wi);
            if (w->removePlayer(*player))
            {   // if player was in queue need to send update
                pdu_queuedelta d;
                d.queueNumber = htons(w->getQueueNumber());
                d.reason = htons(CpduQueueUpdate::QUEUEDELTA_PLAYER_REMOVE);
                strncpy(d.name, player->getUsername(), PDU_STRINGSIZE);
                updates.push_back(d);
            }
        }

        i.next();
    }

    if (updates.size() > 0)
    {
        // Send update to all but 'player'
        CpduQueueUpdate pdu;
        pdu.broadcastQueueUpdates(updates, player);
    }

    if (!force)
    {   
        CpduLogoutConfirmed pdu;
        pdu.sendTo(player->getSocket());
    }

    return true;
}

bool CLounge::queuePlayer(CPlayer* player, u_int16_t queueNumber)
{
  if (shutdown_)
    return false;
  
  // find the waiting list
  CWaitingList* wlist = findWaitingList(queueNumber);
  if (!wlist)
    return false;

  bool rc = queuePlayerLogic(wlist, player);

    /*
    // Let the waiting list decide if the player can be added
    if (!wlist->canAddPlayer(*player))
      return false;
    
    // add player to the list
    rc = wlist->addPlayer(*player);
    
    if (rc)
    { // Broadcast queue update
      CpduQueueUpdate pdu;
      pdu.sendQueueAdd(queueNumber, player);
    }
    else
    { // Tell client queue is full
      CpduQueueFull pdu;
      pdu.sendQueueFull(player->getSocket(), queueNumber);
    }
    */
    
  return rc;
}

bool CLounge::cancelQueue(CPlayer* player, u_int16_t queueNumber)
{
  // find the waiting list
  CWaitingList* wlist = findWaitingList(queueNumber);
  if (!wlist)
    return false;

  bool rc = cancelQueuePlayerLogic(wlist, player);

    /*
    // remove player from the list
    rc = wlist->removePlayer(*player);
  
    // broadcast queue update
    if (rc)
    {
      CpduQueueUpdate pdu;
      pdu.sendQueueRemove(queueNumber, player);
    }
    */

  return rc;
}

// Linear search over all lists, return ptr to
// waiting list with matching number.
CWaitingList* CLounge::findWaitingList(u_int16_t queue)
{
  GameIterator i = CGame::GetGames();
  while (i.more())
  {
    CGame& game = i.current();
    
    WaitingLists& wl = game.getWaitingLists();
    
    WaitingLists::iterator pos = find_if(wl.begin(),
                                         wl.end(),
                                         same_number(queue));
    if (pos != wl.end())
    {
      CWaitingList* wlist = (*pos);
      return wlist;
    }
    
    i.next();
  }
  
  return NULL;
}

// Linear search over all lists, return ptr to
// waiting list with table matching number.
CWaitingList* CLounge::findWaitingListWithTable(u_int16_t tableNumber)
{
    GameIterator i = CGame::GetGames();
    while (i.more())
    {
        CGame& game = i.current();

        WaitingLists& wl = game.getWaitingLists();

        for (WaitingLists::iterator wi = wl.begin(),
                                    we = wl.end();
             wi != we;
             ++wi)
        {
          CWaitingList* wlist = (*wi);

          for (TableList::iterator ti = wlist->getTableList().begin(),
                                   te = wlist->getTableList().end();
               ti != te;
               ++ti)
          {
            if ((*ti).table_->getNumber() == tableNumber)
            {
              return wlist;
            }
          }           
        }

        i.next();
    }

    return NULL;
}


bool CLounge::addTableMaster(u_long addr, u_short port)
{
  /*
  if (master_)
    return false; // support one master 
  
  master_ = CTableMaster::Create(addr, port);
  return (master_ != NULL);
  */
  return true;
}

bool CLounge::addTable(SOCKET sd, u_long addr)
{
  CTable t(sd, addr);
  tables_.push_back(t);
  return true;
}

bool CLounge::tableLogout(SOCKET sd, CTable* table)
{
  // Add the table to the logout list - will be
  // removed from main container on next tick
  loggingOutTables_.push_back(TablePtr(*table));
  
  // Remove the table immediately from the waiting list
  removeTableFromWaitingList(table);
  
  return true;
}

bool CLounge::removeTable(CTable* table)
{
  // Remove table from poller
  SOCKET sd = table->getSocket();
  CPoller::Inst()->removeTableSocket(sd);
  Sys_CloseSocket(sd);
  
  // Remove table from main list
  Tables::iterator pos = find_if(tables_.begin(),
                                 tables_.end(),
                                 same_socket(sd));
  if (pos != tables_.end())
    tables_.erase(pos);
  
  return true;
}

bool CLounge::addWaitingList(int number,
                             u_int16_t queueIndex,
                             u_int16_t queuemax,
                             u_int16_t tablemax,
                             u_int16_t tablelow,
                             u_int16_t tablehigh,
                             int spawnthreshold,
                             int initspawn)
{
    bool rc = false;

    CGame* game = CGame::GetGame(queueIndex);
    if (game)
    {
        CWaitingList* wlist = 
            game->addWaitingList(number,
                                 queuemax,
                                 tablemax,
                                 tablelow,
                                 tablehigh,
                                 spawnthreshold);

        rc = (wlist != NULL);

        if (wlist)
        {
            // Spawn persistent table servers 
            for (int i = 0; i < initspawn; i++)
            {
                spawnTable(game->getGameType(),
                           queueIndex,
                           tablemax,
                           tablelow,
                           tablehigh,
                           game->isHiLoSplit(),
                           true); // XXX: persistent table
            }
        }
    }

    return rc;
}


void CLounge::spawnTable(u_int16_t gameType,
                         u_int16_t queueIndex,
                         u_int16_t tableMax,
                         const CChips& tableLow,
                         const CChips& tableHigh,
                         bool      hiloSplit,
			                   bool      persistent)
{
  // spawn the table directly
  static u_int16_t tableNumber = 1;
  u_short port  = Server::Inst()->getPort();    
  
  // XXX CCHIPS NOTE: cents not handled here
  int low = tableLow.getDollars(),
      high = tableHigh.getDollars();

  CStrOut arg;
  arg << queueIndex << ' '
      << tableNumber << ' '
      << tableMax << ' '
      << low << ' '
      << high << ' '
      << 0 << ' '
      << (port + 1) << ' '
      << (port + 1 + tableNumber) << ' '
      << (hiloSplit ? 1 : 0) << ' '
      << gameType << ' '
      << "-ssl";
  
  int c = (int)ShellExecute(NULL, "open",
                            "c:\\work\\src\\server\\poker_server\\pokerserv.exe",
                            arg.str(),
                            "c:\\work\\src\\server\\poker_server",
                            SW_SHOWNORMAL);
  if (c <= 32)
  {
    Sys_LogError("*** Cannot spawn table! ***\n");
  }
  
  tableNumber++;
}


//
// Add table to the waiting list that has
// - same game type
// - same limits
//
void CLounge::addTableToWaitingList(CTable* table,
                                    u_int16_t gameType)
{
    CGame* game = CGame::GetGame(gameType);

    if (game)
    {
        WaitingLists& wls = game->getWaitingLists();

        WaitingLists::iterator pos = find_if(wls.begin(),
                                             wls.end(),
                                             same_limits(table->getLo(),
                                                         table->getHi()));
        if (pos != wls.end())
        {
            // Add the table to the waiting list
            CWaitingList* wlist = (*pos);
            wlist->addTable(gameType, table);

            // Send queue table added
            CpduQueueUpdate pdu;
            pdu.sendQueueAdd(wlist->getQueueNumber(), table);

            CpduWaitinglistEvent pduEvent;
            pduEvent.sendAdd(wlist->getQueueNumber(), table);

            // Send login stats update
            CpduLoungeStats pduStats;
            pduStats.sendStats(players_.size(), tables_.size());

            // Waiting list has new table, try seating players
            wlist->seatPlayers(table);
        }
        else
        {
            char s[128];
            sprintf(s, "CLounge: No waiting list with limits found: type %d hi: %d lo: %d",
                    gameType, table->getLo(), table->getHi());
            Sys_LogError(s);
        }
    }
    else
    {
        char s[128];
        sprintf(s, "CLounge: No game found for type %d", gameType);
        Sys_LogError(s);
    }
}


void CLounge::addRemoteTable(int queueNumber,
                             int tableNumber,
                             u_long ipaddress,
                             int port)
{
  CWaitingList* wlist = findWaitingList(queueNumber);
  if (!wlist)
  {
    CStrOut buf;
    buf << "CLounge::addRemoteTableToWaitingList: waitinglist "
        << queueNumber << " not found.\n";
    Sys_LogError(buf.str());
    return;
  }

  CTableProxy* remoteTable = new CTableProxy(tableNumber,
                                             ipaddress,
                                             port);
  if (remoteTable)
  {
    remoteTables_.push_back(*remoteTable);
    wlist->addTable(wlist->getGameType(), remoteTable);
  }
}


void CLounge::removeRemoteTable(int queueNumber,
                                int tableNumber)
{
  CWaitingList* wlist = findWaitingList(queueNumber);
  if (!wlist)
  {
    CStrOut buf;
    buf << "CLounge::removeRemoteTable: waitinglist "
        << queueNumber << " not found.\n";
    Sys_LogError(buf.str());
    return;
  }

  // Remove the table from the waiting list
  wlist->removeTable(tableNumber);

  // Remove the table from the main tables list
  RemoteTables::iterator new_end = remove_if(remoteTables_.begin(),
                                             remoteTables_.end(),
                                             same_number(tableNumber));
  if (new_end != remoteTables_.end())
    remoteTables_.erase(new_end, remoteTables_.end());
}


//
// Remove table from the waiting list that has
// - same game type
// - same limits
//
void CLounge::removeTableFromWaitingList(CTable* table)
{
    CGame* game = CGame::GetGame(table->getGameType());

    if (game)
    {
        WaitingLists& wls = game->getWaitingLists();

        WaitingLists::iterator pos = find_if(wls.begin(),
                                             wls.end(),
                                             same_limits(table->getLo(),
                                                         table->getHi()));
        if (pos != wls.end())
        {
            CWaitingList* wlist = (*pos);

            TableList& tlist = wlist->getTableList();
            TableList::iterator tpos = find_if(tlist.begin(),
                                               tlist.end(),
                                               same_socket(table->getSocket()));
            if (tpos != tlist.end())
            {   
                tlist.erase(tpos);

                // Send queue table removed
                CpduQueueUpdate pdu;
                pdu.sendQueueRemove(wlist->getQueueNumber(), table);

                CpduWaitinglistEvent pduEvent;
                pduEvent.sendRemove(wlist->getQueueNumber(), table);
            }
            else
            {
                printf("CLounge: Table not in waiting list: type %d lo: %d hi: %d",
                        table->getGameType(), table->getLo(), table->getHi());
            }
        }
        else
        {
            char s[128];
            sprintf(s, "CLounge: No waiting list with limits found: type %d lo: %d hi: %d",
                    table->getGameType(), table->getLo(), table->getHi());
            Sys_LogError(s);
        }
    }
    else
    {
        char s[128];
        sprintf(s, "CLounge: No game found for type %d", table->getGameType());
        Sys_LogError(s);
    }
}

CTable* CLounge::findTable(u_int16_t tableNumber)
{
    Tables::iterator pos = find_if(tables_.begin(),
                                   tables_.end(),
                                   same_number(tableNumber));
    if (pos != tables_.end())
    {
        CTable& table = *pos;
        return &table;
    }

    return NULL;
}

void CLounge::playerSeated(const char* username,
                           CTable*     table)
{
    table->playerSeated(username);
}

void CLounge::playerUnseated(const char* username,
                             CTable*     table)
{
    table->playerUnseated(username);
}


void CLounge::dumpConfig()
{
    GameIterator i = CGame::GetGames();
    while (i.more())
    {
        CGame& game = i.current();
        printf("Game %d waiting lists:\n", game.getGameSubType());

        WaitingLists& wl = game.getWaitingLists();
        for (WaitingLists::iterator wi = wl.begin(),
                                    we = wl.end();
             wi != we;
             ++wi)
        {
            CWaitingList* w = (*wi);
            printf("    List %d: max: %d limits: %s\n",
                   w->getQueueNumber(), w->getTableMax(),
                   w->getTitle().c_str());
            if (w->getTableList().size() != 0 ||
                w->getPlayers().size() != 0)
            {
                printf("       tables: %d players: %d\n",
                       w->getTableList().size(), w->getPlayers().size());
            }
        }

        i.next();
    }
}


CPlayer* CLounge::findPlayer(const char* username, bool searchRemote)
{
  CPlayer* player = NULL;
  
  // Convert name to lowercase since
  // names are case insensitive
  string checkName(username);
  tolower(checkName);
  
  Players::iterator pos = players_.find(checkName);
  if (pos != players_.end())
  {
    player = &(*pos).second;
  }

  if (!player && searchRemote)
  {
    RemotePlayers::iterator pos = remotePlayers_.find(checkName);
    
    if (pos != remotePlayers_.end())
      player = &(*pos).second;
  }
  
  return player;
}


long CLounge::getMaxPlayers() const
{
  long maxPlayers = 0;
  CdbInterface::Inst()->getMaxPlayers(maxPlayers);
  return maxPlayers;
}


bool CLounge::queuePlayerLogic(int queueNumber, const char* username)
{
  CPlayer* player = findPlayer(username);
  CWaitingList* wlist = findWaitingList(queueNumber);
  if (!wlist || !player)
    return false;
  else
    return queuePlayerLogic(wlist, player);
}

bool CLounge::queuePlayerLogic(CWaitingList* wlist,
                               CPlayer* player)
{
  bool rc = true;

  // Let the waiting list decide if the player can be added
  if (!wlist->canAddPlayer(*player))
    return false;
    
  // add player to the list
  rc = wlist->addPlayer(*player);

  if (rc)
  {
    CpduQueueUpdate pdu;
    pdu.sendQueueAdd(wlist->getQueueNumber(), player);
  }
  else
  { // Tell client queue is full
    CpduQueueFull pdu;
    pdu.sendQueueFull(wlist->getQueueNumber(), player);
  }

  return rc;
}

bool CLounge::cancelQueuePlayerLogic(int queueNumber, const char* username)
{
  CPlayer* player = findPlayer(username);
  CWaitingList* wlist = findWaitingList(queueNumber);
  if (!wlist || !player)
    return false;
  else
    return cancelQueuePlayerLogic(wlist, player);
}

bool CLounge::cancelQueuePlayerLogic(CWaitingList* wlist, CPlayer* player)
{
  bool rc = true;

  // remove player from the list
  rc = wlist->removePlayer(*player);
  
  // broadcast queue update
  if (rc)
  {
    CpduQueueUpdate pdu;
    pdu.sendQueueRemove(wlist->getQueueNumber(), player);
  }

  return rc;
}




// A player has logged in / out, notify Load Balancer
// and/or other lounges.
void CLounge::notifyOthers(NotifyReason reason, const char* username)
{
  CpduLoungeStats pduStats;
  pduStats.sendStats(players_.size(), tables_.size());

  CLoadBalancer* lb = CLoadBalancer::Inst();

  if (lb)
  {
    if (reason == AddPlayer)
    {
      lb->sendPlayerLoggedIn(username);
    }
    else if (reason == RemovePlayer)
    {
      lb->sendPlayerLoggedOut(username);
    }
  }
}


void CLounge::notifyOthers(NotifyReason reason, int number)
{
  CpduLoungeStats pduStats;
  pduStats.sendStats(players_.size(), tables_.size());

  if (reason == AddTable)
  {
  }
  else if (reason == RemoveTable)
  {
  }
  else if (reason == AddTournament)
  {
  }
  else if (reason == RemoveTournament)
  {
  }
}


CPlayer* CLounge::addRemotePlayer(const char* username)
{
  CPlayerProxy* player = NULL;
  

  // First check that he's not already 
  // in the container

  // Convert name to lowercase since
  // names are case insensitive
  string checkName(username);
  tolower(checkName);
  RemotePlayers::iterator pos = remotePlayers_.find(checkName.c_str());
  if (pos != remotePlayers_.end())
    player = &(*pos).second;
  
  if (player)
    return player;

  pos = remotePlayers_.insert(RemotePlayers::value_type(checkName,
                                                        CPlayerProxy(username))).first;
  if (pos != remotePlayers_.end())
    player = &(*pos).second;

  return player;
}

bool CLounge::removeRemotePlayer(const char* username)
{
  // Convert name to lowercase since
  // names are case insensitive
  string lowercase(username);
  tolower(lowercase);

  return remotePlayers_.erase(lowercase) > 0;
}

CWaitingList* FindWaitinglistWithPlayer(CPlayer* player)
{
  GameIterator games = CGame::GetGames();
  while (games.more())
  {
    CGame& game = games.current();
    WaitingLists& wls = game.getWaitingLists();

    for (WaitingLists::iterator wli = wls.begin(),
                                wle = wls.end();
         wli != wle;
         ++wli)
    {
      CWaitingList* wlist = (*wli);

      PlayerList& inWaitinglist = wlist->getPlayers();
      PlayerPtr ptr(*player);

      if (find(inWaitinglist.begin(),
               inWaitinglist.end(),
               ptr) != inWaitinglist.end())
      {
        // The player is in this waiting list
        return wlist;
      }
    }
    games.next();
  }

  return NULL;
}

struct not_in_waitinglists : 
  public unary_function<RemotePlayers::value_type&, bool>
{ 
  bool operator() (RemotePlayers::value_type& item)
  {
    return FindWaitinglistWithPlayer(&(item.second)) != NULL;
  }
};

void CLounge::collectGarbage()
{
  // Forget about those remote players that are
  // not in our waiting lists any more
  RemotePlayers::iterator it = remotePlayers_.begin();
  while (it != remotePlayers_.end())
  {
    if (FindWaitinglistWithPlayer(&((*it).second)) == NULL)
    {
      RemotePlayers::iterator dead(it);
      ++it;
      remotePlayers_.erase(dead);
    }
    else
    {
      ++it;
    }
  }

/*
  remove_if(remotePlayers_.begin(),
            remotePlayers_.end(),
            not_in_waitinglists());
*/
}

//
// Financials
//

bool CLounge::doCredit(CPlayer *player, u_int16_t amount)
{
    return dbase_->addCreditTrans(player, amount);
}

bool CLounge::doDebit(CPlayer *player, u_int16_t amount, u_int16_t index)
{
    return dbase_->addDebitTrans(player, amount, index);
}

char* CLounge::checkTransaction(CPlayer *player)
{
    return dbase_->checkTransaction(player);
}

