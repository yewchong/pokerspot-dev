/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRoutePlayerPdu
  Created  : 10/02/2001

  OVERVIEW : Class CpduRoutePlayerPdu acts as a wrapper class around other
             PDU objects that need to be routed to specific players.

 ****************************************************************************/

#include "loadbalancer/pdurouteplayerpdu.h"
#include "loadbalancer/loadbalancer.h"
#include "lounge/lounge.h"
#include "sys.h"


REGISTER_INCOMING_PDU(PDU_LB_RoutePlayerPDU, CpduRoutePlayerPdu);


CpduRoutePlayerPdu::CpduRoutePlayerPdu()
{
  type_ = PDU_LB_RoutePlayerPDU;
  length_ = PDU_HEADER_SIZE + PDU_STRINGSIZE;
  stuffHeader();
}

// Wrap 'pdu' inside this PDU and send it.
int CpduRoutePlayerPdu::routePdu(Cpdu* pdu, const char* username)
{
  // Add player's name to the PDU for routing
  // purposes
  char* buf = value_ + PDU_HEADER_SIZE;
  strncpy(buf, username, PDU_STRINGSIZE);
  buf += PDU_STRINGSIZE;

  if (length_ + pdu->getLength() >= MAXBUFFERSIZE)
  {
    Sys_LogError("CpduRoutePlayerPdu: length > MAXBUFFERSIZE (1)");
    return -1;
  }

  // Copy the PDU data
  memcpy(buf, pdu->getValue(), pdu->getLength());

  // Adjust PDU length
  length_ += pdu->getLength();
  stuffHeader();

  // XXX TODO: route it directly to the proper Lounge?

  // Send the PDU to the Load Balancer which will forward
  // it to the appropriate Lounge Server.
  if (CLoadBalancer::Inst())
    return sendTo(CLoadBalancer::Inst()->getSocket());
  else
    return 0;
}

// Extract the wrapped PDU and send it to the
// designated player.
void CpduRoutePlayerPdu::grok(SOCKET sd, CLoadBalancer* lb)
{
  int rc = 0;

  char* buf = value_ + PDU_HEADERSIZE; 
  char sz_username[PDU_STRINGSIZE+1];
  memcpy(sz_username, buf, PDU_STRINGSIZE);
  buf += PDU_STRINGSIZE;

  CPlayer* player = CLounge::Inst()->findPlayer(sz_username);

  if (player)
  {
    // Create the routed PDU from the buffer and
    // send it to the player
   
    pduh_t* pduheader = reinterpret_cast<pduh_t*>(buf);
    u_int16_t type = ntohs(pduheader->type);
    u_int16_t length = ntohs(pduheader->length);

    assert(length_ == (PDU_HEADERSIZE + PDU_STRINGSIZE + length));

    if (length >= MAXBUFFERSIZE)
    {
      Sys_LogError("CpduRoute: length > MAXBUFFERSIZE(2)");
    }
    else
    {
      Cpdu pdu(type, length, buf);
      rc = pdu.sendTo(player->getSocket());
    }
  }

  delete this;
}
