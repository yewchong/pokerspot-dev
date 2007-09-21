/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : Class CPlayerProxy
  Created  : 10/03/2001

  OVERVIEW : This CPlayer subclass is used as a proxy to represent
             a player who is logged in to another Lounge Server.

 ****************************************************************************/


#include "loadbalancer/playerproxy.h"
#include "loadbalancer/pdurouteplayerpdu.h"


int CPlayerProxy::send(Cpdu& pdu)
{
  CpduRoutePlayerPdu wrapper;

  // The wrapper prepends additional routing data
  // and sends the PDU forward.
  return wrapper.routePdu(&pdu, getUsername());
}


// PlayerProxies are not pinged! 
CPingCounter* CPlayerProxy::getPingCounter()
{
  return NULL;
}
