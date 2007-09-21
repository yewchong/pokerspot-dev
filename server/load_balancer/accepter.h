#ifndef __lb_accepter_h__
#define __lb_accepter_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPoller
  Created  : 09/21/2001

  OVERVIEW : This class polls sockets for incoming data.

 ****************************************************************************/

#include "common/sync.h"
#include <list>


class CAccepter
{
public:
  CAccepter();
  
  void beginAcceptPlayer(SOCKET listensd, struct sockaddr_in*);

  void beginAcceptLounge(SOCKET listensd, struct sockaddr_in*);
  void acceptedLounge   (SOCKET acceptedsd, const struct sockaddr_in&);
  int  getAcceptedLounges(int& r_maxfd, fd_set* p_allset);
  
  static CAccepter* Inst();
  
  void incThreads();
  void decThreads();
  

  struct AcceptItem
  {
    SOCKET sock_;
    struct sockaddr_in cliaddr_;
    
    AcceptItem(SOCKET s, const struct sockaddr_in& a)
      : sock_(s)
    {
      memcpy(&cliaddr_, &a, sizeof(struct sockaddr_in));
    }
    AcceptItem& operator=(const AcceptItem& rhs)
    {
      if (this != &rhs)
      {
        sock_ = rhs.sock_;
        memcpy(&cliaddr_, &rhs.cliaddr_, sizeof(struct sockaddr_in));
      }
      return *this;
    }
  };
  
private:
  static CAccepter* pInst_;
  
  int                   numThreads_, maxThreads_;
  std::list<AcceptItem> acceptedSockets_;
  Mutex                 mutex_;
};

struct AcceptInfo
{
  SOCKET listensd;    
};


#endif
