/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoadBalancer
  Created  : 22/09/2001

  OVERVIEW : This class manages the connected Lounge Servers
             and does the load balancing logic.

 ****************************************************************************/

#include "loadbalancer/loadbalancer.h"
#include "poller.h"
#include "pdu/pduplayerhandshake.h"
#include "pdu/pduredirectlogin.h"
#include "pdu/pdunotify.h"
#include "common/sys.h"
#include <iostream.h>
#include <algorithm>
USING_NS_T(std, list<LoungePtr>);
USING_NS_T(std, find_if);


void tolower(string& s)
{
  for (int i = s.size() - 1; i >= 0; i--)
    s[i] = tolower(s.at(i));
}


// Global CLoadBalancer instance
static CLoadBalancer g_loadBalancer;


CLoadBalancer* CLoadBalancer::pInst_ = NULL;


CLoadBalancer::CLoadBalancer()
  :
  shutdown_(0)
{
  pInst_ = this;
}

CLoadBalancer::~CLoadBalancer()
{
  pInst_ = NULL;
}

void CLoadBalancer::tick(long now)
{
  processLogouts(now);
}

void CLoadBalancer::processLogouts(long now)
{
  bool sendUpdate = false;
  
  //
  // Process logging out lounges
  //
  list<LoungePtr>::iterator li = loggingOutLounges_.begin();
  
  while (li != loggingOutLounges_.end())
  {
    sendUpdate = true;
    
    CLounge* lounge = (*li).lounge_;
    cout << " *** Removing lounge " << lounge->getNumber()
         << " socket " << lounge->getSocket();
    
    removeLounge(lounge);
    
    // NOTE: 'lounge' points to destroyed data
    
    // Remove it from logging out list
    li = loggingOutLounges_.erase(li);
  }
  
  if (sendUpdate)
  {
    // Send login stats update if something was removed
    //CpduLoungeStats pduStats;
    //pduStats.sendStats(players_.size(), lounges_.size());
  }
  
}

LoungeIterator CLoadBalancer::getLounges()
{
  return LoungeIterator(lounges_.begin(), lounges_.end());
}

LoungeIteratorC CLoadBalancer::getLounges() const
{
  return LoungeIteratorC(lounges_.begin(), lounges_.end());
}

int CLoadBalancer::handlePlayerConnection(SOCKET sd)
{
  // This member function is called from new player
  // connection threads.
  SingleLock l(&playerMutex_);
  if (!l.lock())
    return -1;

  // Get the player handshake data
  CpduPlayerHandshake pdu;
  if (pdu.readHandshake(sd) == -1)
    return -1;

  int rc = 0;

  if (checkClientVersion(pdu.getPlatform(),
                         pdu.getChecksum()))
  {
    //
    // Version is ok, redirect player to the
    // Lounge Server that has smallest load
    //
    CLounge* lounge = decideLoungeServer(pdu.getUsername());
    if (lounge)
    {
      rc = sendPlayerToLounge(sd, pdu.getUsername(), lounge);
    }
    else
    {
      // There are no Lounge Servers currently available -
      // notify player.

      // TODO: get the message to show from the database.
      CpduNotify pdu;
      pdu.sendNotifyMessage(sd,
                            "The server is currently offline.",
                            CpduNotify::CloseConnection);
    }
  }
  else
  {
    //
    // Client has an old version, initiate
    // the upgrade process
    //
    rc = sendUpgradeRequest(sd);
  }

  return rc;
}

bool CLoadBalancer::checkClientVersion(u_int16_t platform,
                                       const u_byte_t* clientChecksum)
{
/*
  // if checksum does not match, initiate auto-upgrade
  u_byte_t* serverChecksum = AutoUpgrade::ComputeChecksum(platform, configFile);
  
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
*/
  
  return true;
}

bool CLoadBalancer::addLounge(SOCKET sd, u_long addr)
{
  CLounge l(sd, addr, lounges_.size() + 1);
  lounges_.push_back(l);
  return true;
}

bool CLoadBalancer::removeLounge(SOCKET sd, CLounge* lounge)
{
  // Add the lounge to the logout list - it will be
  // removed from main container on next tick
  loggingOutLounges_.push_back(LoungePtr(*lounge));

  return true;
}

bool CLoadBalancer::removeLounge(CLounge* lounge)
{
  // Remove lounge from poller
  SOCKET sd = lounge->getSocket();
  CPoller::Inst()->removeLoungeSocket(sd);
  Sys_closesocket(sd);
  
  // Remove all associations from Player <-> Lounge
  // for this lounge.

  // NOTE: the below is safe: when an item is destroyed
  // from a hash_map, only the iterator pointing to the
  // erased item is invalidated.
  Player2Lounge::iterator it = player2Lounge_.begin();
  while (it != player2Lounge_.end())
  {
    if ((*it).second == lounge)
    {
      Player2Lounge::iterator dead(it);
      ++it;
      player2Lounge_.erase(dead);
    }
    else
    {
      ++it;
    }
  }
  
  // Remove lounge from main list
  Lounges::iterator pos = find_if(lounges_.begin(),
                                  lounges_.end(),
                                  same_socket(sd));

  if (pos != lounges_.end())
    lounges_.erase(pos);
  
  return true;
}


CLounge* CLoadBalancer::findLounge(int loungeNumber)
{
  Lounges::iterator pos = find_if(lounges_.begin(),
                                  lounges_.end(),
                                  same_number(loungeNumber));
  if (pos != lounges_.end())
  {
    CLounge& lounge = *pos;
    return &lounge;
  }

  return NULL;
}


//
// Very Simple Load Balancing (TM).
// - If player is already registered with one of the
//   lounges, return that lounge.
// - Otherwise return the lounge that has the
//   smallest number of players.
//
CLounge* CLoadBalancer::decideLoungeServer(const char* username)
{
  CLounge* best = findLoungeByPlayer(username);
  if (best != NULL)
    return best;

  for (LoungeIterator li = getLounges(); li.more(); li.next())
  {
    CLounge* lounge = li.current();
    if (!best || lounge->getNumPlayers() < best->getNumPlayers())
      best = lounge;
  } 

  return best;
}


int CLoadBalancer::sendPlayerToLounge(SOCKET sd,
                                      const char* username,
                                      CLounge* lounge)
{
  registerLoungePlayer(lounge, username);

  CpduRedirectLogin pdu;
  return pdu.sendRedirectLogin(sd,
                               lounge->getIpAddress(),
                               lounge->getPort());
}


int CLoadBalancer::sendUpgradeRequest(SOCKET sd)
{
  // CpduUpgradeRequest pdu;
  // pdu.sendUpgradeRequest(sd);
  return 0;
}


int CLoadBalancer::numLoungesLoggedIn(const CLounge* exceptThis) const
{
  int rc = 0;

  for (LoungeIteratorC li = getLounges(); li.more(); li.next())
  {
    const CLounge* lounge = li.current();

    if (lounge != exceptThis && lounge->isLoggedIn())
    {
      rc++;
    }
  } 

  return rc;
}


CLounge* CLoadBalancer::findLoungeByPlayer(const char* username)
{
  CLounge* lounge = NULL;

  // Convert name to lowercase since
  // names are case insensitive
  string checkName(username);
  tolower(checkName);

  // Lookup from the username <-> Lounge Server table
  Player2Lounge::iterator pos = player2Lounge_.find(checkName.c_str());
  if (pos != player2Lounge_.end())
  {
    lounge = (*pos).second;
  }

  return lounge;
}

// Register Player <-> Lounge association (i.e., player
// 'username' is logged in to Lounge).
bool CLoadBalancer::registerLoungePlayer(CLounge* lounge, const char* username)
{
  string saveName(username);
  tolower(saveName);
  bool b = player2Lounge_.insert(
             Player2Lounge::value_type(saveName.c_str(), lounge)).second;

//  cout << "Registered player " << username << endl;
//  std::for_each(player2Lounge_.begin(),
//                player2Lounge_.end(),
//                print_contents());

  return b;
}


// Unregister Player <-> Lounge association.
bool CLoadBalancer::unregisterLoungePlayer(CLounge* lounge, const char* username)
{
  string n(username);
  tolower(n);
  bool b = player2Lounge_.erase(n.c_str()) > 0;

//  cout << "Unregistered player " << username << endl;
//  std::for_each(player2Lounge_.begin(),
//                player2Lounge_.end(),
//                print_contents());

  return b;

}


CLounge* CLoadBalancer::findLoungeByWaitinglist(int wlistNumber)
{
  int loungeNumber = getWaitinglistOwner(wlistNumber);
  return findLounge(loungeNumber);
}


int CLoadBalancer::getWaitinglistOwner(int queueNumber)
{
  int numWaitinglists = CWG_NUM_WAITINGLISTS;
  int numLounges = lounges_.size();
  if (numLounges == 0 || numWaitinglists == 0)
    return 0;

  int owner = 1;
  float curr = 1.0f,
        incr = float(numLounges) / float(numWaitinglists); 

  for (int i = 0; i < queueNumber; i++)
  {
    owner = (int)curr;
    curr += incr;
  }
  
  return owner;
}


CLounge* CLoadBalancer::findLoungeByTable(int number)
{
  // not yet implemented
  assert(FALSE);
  return NULL;
}

int CLoadBalancer::getTableOwner(int table)
{
  // not yet implemented
  assert(FALSE);
  return 0;
}
