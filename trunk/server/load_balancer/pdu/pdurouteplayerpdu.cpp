/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRoutePlayerPdu
  Created  : 10/02/2001

  OVERVIEW : Class CpduRoutePlayerPdu acts as a wrapper class around other
             PDU objects that need to be routed to specific players.

 ****************************************************************************/

#include "pdu/pdurouteplayerpdu.h"
#include "loadbalancer/loadbalancer.h"
#include "common/errorlog.h"

REGISTER_INCOMING_PDU(PDU_LB_RoutePlayerPDU, CpduRoutePlayerPdu);


CpduRoutePlayerPdu::CpduRoutePlayerPdu()
{
  type_ = PDU_LB_RoutePlayerPDU;
  length_ = PDU_HEADER_SIZE + PDU_STRINGSIZE;
  stuffHeader();
}

// Route the PDU to the Lounge Server in which player
// is logged in.
void CpduRoutePlayerPdu::grok(SOCKET sd, CLounge* senderLounge)
{
  int rc = 0;

  char* buf = value_ + PDU_HEADERSIZE; 
  char sz_username[PDU_STRINGSIZE+1];
  ::ZeroMemory(sz_username, sizeof(sz_username));
  memcpy(sz_username, buf, PDU_STRINGSIZE);
 
  CLounge* targetLounge = CLB::Inst()->findLoungeByPlayer(sz_username);

  if (targetLounge)
  {
    rc = sendTo(targetLounge->getSocket());
  }
  else
  {
    Err::Log() << "CpduRoutePlayerPdu: player " << sz_username 
               << " not found.\n";
  }

  delete this;
}
