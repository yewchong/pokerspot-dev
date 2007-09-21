/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRouteLoungePdu
  Created  : 10/02/2001

  OVERVIEW : Class CpduRouteLoungePdu acts as a wrapper class around other
             PDU objects that need to be routed to a specific Lounge Server.

 ****************************************************************************/

#include "pdu/pdurouteloungepdu.h"
#include "loadbalancer/loadbalancer.h"
#include "common/errorlog.h"


REGISTER_INCOMING_PDU(PDU_LB_RouteLoungePDU, CpduRouteLoungePdu);


CpduRouteLoungePdu::CpduRouteLoungePdu()
{
  type_ = PDU_LB_RouteLoungePDU;
  length_ = PDU_HEADER_SIZE + PDU_STRINGSIZE;
  stuffHeader();
}


// Route the PDU to the designated Lounge Server.
void CpduRouteLoungePdu::grok(SOCKET sd, CLounge* sender)
{
  CLoadBalancer* lb = CLB::Inst();
  int rc = 0;
  char* buf = value_ + PDU_HEADERSIZE; 

  pdu_routingdata* routingdata = reinterpret_cast<pdu_routingdata*>(buf);
  int flag = ntohs(routingdata->flag);
  int number = ntohs(routingdata->number);

  CLounge* targetLounge = NULL;
  
  if (flag == WaitinglistNumber)
  {
    targetLounge = lb->findLoungeByWaitinglist(number);
  }
  else if (flag == TableNumber)
  {
    targetLounge = lb->findLoungeByTable(number);
  }
  else
  {
    Err::Log() << "CpduRouteLoungePdu: unknown flag: " << flag << '\n';
  }

  if (targetLounge)
  {
    rc = sendTo(targetLounge->getSocket());
  }

  delete this;
}
