#ifndef __lounge_pduloungestats_h__
#define __lounge_pduloungestats_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoungeStats
  Created  : 04/21/2000

  OVERVIEW : Lounge statistics PDU.

             Lounge Server sends this PDU to the Load Balancer
             to update the current statistics of this Lounge.
             Load Balancer computes the grand total and sends
             it to all Lounge Servers.

 ****************************************************************************/

#include "pdu.h"


class CLoadBalancer;


class CpduLoungeStats : public Cpdu
{
public:
  CpduLoungeStats();
  int sendStats(u_int32_t players, u_int16_t tables);
  void grok(SOCKET sd, CLoadBalancer*);
};


#endif
