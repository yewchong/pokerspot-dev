#ifndef __lb_poller_h__
#define __lb_poller_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPoller
  Created  : 09/21/2001

  OVERVIEW : This class polls sockets for incoming data.

 ****************************************************************************/

#include "common/compat.h"

class CLounge;

class CPoller
{
public:
  CPoller(SOCKET sd_player, SOCKET sd_lounge);
  ~CPoller();

  int poll();
  int poll(struct timeval*);

  void incomingPdu        (SOCKET, CLounge*);
  void addLoungeSocket    (SOCKET);
  void removeLoungeSocket (SOCKET);

  static CPoller* Inst() { return pInst_; }

protected:
  void newPlayerConnection(SOCKET);
  void newLoungeConnection(SOCKET);

private:
  SOCKET sd_player_;
  SOCKET sd_lounge_;
  int    max_listen_sd_;
  fd_set listensds_;

  static CPoller* pInst_;
};


#endif
