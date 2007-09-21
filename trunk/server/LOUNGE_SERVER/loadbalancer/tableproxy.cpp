/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : Class CTableProxy
  Created  : 10/09/2001

  OVERVIEW : This CTable subclass is used as a proxy to represent
             a table logged in to another Lounge Server.

 ****************************************************************************/


#include "loadbalancer/tableproxy.h"
#include "loadbalancer/pdurouteloungepdu.h"


CTableProxy::CTableProxy(int number,
                         u_long ipaddress,
                         int port)
  :
  CTable(INVALID_SOCKET, 0)
{}


int CTableProxy::send(Cpdu& pdu)
{
  CpduRouteLoungePdu wrapper;

  // The wrapper prepends additional routing data
  // and sends the PDU forward.
  //return wrapper.routePdu(&pdu, getNumber());
  return 0;
}
