/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoungeProxy
  Created  : 28/09/2001

  OVERVIEW : This class represents one connected Lounge Server.

 ****************************************************************************/

#include "loadbalancer/loungeproxy.h"


CLoungeProxy::CLoungeProxy(SOCKET sd, u_long addr)
  :
  sd_          (sd),
  number_      (0),
  addr_        (addr),
  port_        (0),
  networkError_(0)
{}

CLoungeProxy::CLoungeProxy(const CLoungeProxy& rhs)
  :
  sd_          (rhs.sd_),
  number_      (rhs.number_),
  addr_        (rhs.addr_),
  port_        (rhs.port_),
  networkError_(rhs.networkError_)
{}

CLoungeProxy& CLoungeProxy::operator = (const CLoungeProxy& rhs)
{
  if (this != &rhs)
  {
    sd_ = rhs.sd_;
    number_ = rhs.number_;
    addr_ = rhs.addr_;
    port_ = rhs.port_;
    networkError_ = rhs.networkError_;
  }
  return *this;
}

