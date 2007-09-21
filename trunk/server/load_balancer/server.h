#ifndef __lb_server_h__
#define __lb_server_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CServer
  Created  : 09/21/2000

  OVERVIEW : CServer class contains the main loop of the Load Balancer.

 ****************************************************************************/

#include "common/compat.h"

class CServer
{
public:
  CServer();
  ~CServer();
  
  static CServer* Inst();
  
  int  initialize(int argc, char** argv);
  int  readConfig();
  int  processConfig();
  
  int  mainLoop();
  void shutdown();

  bool getQuit() const;
  void setQuit(bool);

  u_long getLocalHostIpAddress() const;
  
private:
  SOCKET sd_lounge_, sd_player_;
  bool   quit_;
  u_long ipaddress_; // ip address of this host

  static CServer* pInst_;
};


inline CServer* CServer::Inst()
{
  return pInst_; 
}

inline bool CServer::getQuit() const
{
  return quit_;
}

inline void CServer::setQuit(bool b)
{
  quit_ = b;
}


#endif
