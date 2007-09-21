#ifndef __lounge_pdurouteplayerpdu_h__
#define __lounge_pdurouteplayerpdu_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRoutePlayerPdu
  Created  : 10/02/2001

  OVERVIEW : Class CpduRoutePlayerPdu acts as a wrapper class around other
             PDU objects that need to be routed to specific players.

 ****************************************************************************/

#include "pdu/pdu.h"


class CpduRoutePlayerPdu : public Cpdu
{
public:
  CpduRoutePlayerPdu();

  int routePdu(Cpdu* pdu, const char* username);
  void grok(SOCKET sd, CLoadBalancer*);
};


#endif