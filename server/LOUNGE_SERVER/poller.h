#ifndef __lounge_poller_h__
#define __lounge_poller_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CPoller
  Created  : 04/16/2000

  OVERVIEW : Socket poller class.

 ****************************************************************************/

#include "compat.h"

class CPlayer;
class CTable;
class CLoadBalancer;

class CPoller
{
public:
  CPoller(SOCKET sd_player, SOCKET sd_table);
  ~CPoller();
  int poll();
  int poll(struct timeval*);
  
  void incomingPdu(SOCKET sd, CPlayer*);
  void addPlayerSocket(SOCKET sd);
  void removePlayerSocket(SOCKET sd);
  
  void incomingPdu(SOCKET sd, CTable*);
  void addTableSocket(SOCKET sd);
  void removeTableSocket(SOCKET sd);
  
  void incomingPdu(SOCKET sd, CLoadBalancer*);
  
  static CPoller* Inst() { return pInst_; }
  
private:
  SOCKET sd_player_;
  int    max_player_sd_;
  fd_set player_readfds_;
  
  SOCKET sd_table_;
  int    max_table_sd_;
  fd_set table_readfds_;
  
  static CPoller* pInst_;
};


#endif