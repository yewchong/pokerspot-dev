#ifndef __lounge_loadbalancer_h__
#define __lounge_loadbalancer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoadBalancer
  Created  : 09/26/2001

  OVERVIEW : This class represents the connection to Load Balancer in
             the Lounge Server.

 ****************************************************************************/

#include "def.h"
#include "compat.h"

class CLoadBalancer
{
public:
  ~CLoadBalancer();

  static CLoadBalancer* Inst();
  static CLoadBalancer* ConnectToLoadBalancer(u_long addr, u_short port);
  int closeConnection();

  int sendLoungeLogin (u_short port);
  int sendLoungeUpdate(int numplayers);
  int sendLoungeLogout();
  
  int sendPlayerLoggedIn (const char* username);
  int sendPlayerLoggedOut(const char* username);

  void setWaitinglistOwner(int waitinglist, int lounge);
 
  SOCKET getSocket() const;
  
private:
  
  CLoadBalancer(SOCKET);
  
  SOCKET sd_;
  
  static CLoadBalancer* pInst_;
};

inline SOCKET CLoadBalancer::getSocket() const
{
  return sd_;
}

inline CLoadBalancer* CLoadBalancer::Inst()
{
  return pInst_;
}

#endif