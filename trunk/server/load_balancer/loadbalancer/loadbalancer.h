#ifndef __lb_loadbalancer_h__
#define __lb_loadbalancer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoadBalancer
  Created  : 22/09/2001

  OVERVIEW : This class manages the connected Lounge Servers
             and does the load balancing logic.

 ****************************************************************************/

#include "common/compat.h"
#include "common/sync.h"
#include "loadbalancer/lounge.h"
#include "loadbalancer/loungeiterator.h"
#include "loadbalancer/loungeptr.h"
#include "loadbalancer/player2lounge.h"
#include <list>


class CLoadBalancer
{
public:
  CLoadBalancer               ();
  ~CLoadBalancer              ();
  static CLoadBalancer* Inst  ();


  //
  // Lounge Server management
  //
  bool addLounge              (SOCKET, u_long addr);
  bool removeLounge           (SOCKET sd, CLounge* lounge);
  void processLogouts         (long now);
  
  CLounge* findLounge         (int loungeNumber);
  LoungeIterator getLounges   ();
  LoungeIteratorC getLounges  () const;
  int numLounges              () const;
  int numLoungesLoggedIn      (const CLounge* exceptThis = NULL) const;
 
  void tick                   (long now);

  void setShutdown            (int shutdown);
  int  getShutdown            () const;
  
  int  handlePlayerConnection (SOCKET sd);
  bool checkClientVersion     (u_int16_t platform,
                               const u_byte_t* checksum);
  CLounge* decideLoungeServer (const char* username);
  int  sendPlayerToLounge     (SOCKET sd, const char* username, CLounge*);
  int  sendUpgradeRequest     (SOCKET sd);

  bool registerLoungePlayer   (CLounge* lounge, const char* username);
  bool unregisterLoungePlayer (CLounge* lounge, const char* username);
  CLounge* findLoungeByPlayer (const char* username);


  CLounge* findLoungeByWaitinglist(int number);
  int      getWaitinglistOwner    (int queueNumber);
  CLounge* findLoungeByTable      (int number);
  int      getTableOwner          (int table);
 
private:
  bool removeLounge(CLounge*);
        
  // Connected Lounge Servers:
  Lounges lounges_;
  // Lounge Servers logging out are kept in separate list
  list<LoungePtr> loggingOutLounges_;

  // Player <-> Lounge associations. This table maintains
  // information about which player is logged in to which
  // Lounge. This information is needed for routing.
  Player2Lounge player2Lounge_;

  int shutdown_;
  
  static CLoadBalancer* pInst_;

  Mutex playerMutex_;
};

// Use shorthand type name
typedef CLoadBalancer CLB;


inline CLoadBalancer* CLoadBalancer::Inst() 
{
  return pInst_;
}

inline int CLoadBalancer::numLounges() const
{   
  return lounges_.size();
}

inline void CLoadBalancer::setShutdown(int shutdown)
{
  shutdown_ = shutdown;
}

inline int CLoadBalancer::getShutdown() const
{
  return shutdown_;
}

extern void tolower(string&);

#endif
