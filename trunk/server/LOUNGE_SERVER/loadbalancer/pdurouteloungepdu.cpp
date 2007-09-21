/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRouteLoungePdu
  Created  : 10/02/2001

  OVERVIEW : Class CpduRouteLoungePdu acts as a wrapper class around other
             PDU objects that need to be routed to a specific Lounge Server.

 ****************************************************************************/

#include "loadbalancer/pdurouteloungepdu.h"
#include "loadbalancer/loadbalancer.h"
#include "loadbalancer/playerproxy.h"
#include "lounge/lounge.h"
#include "sys.h"


REGISTER_INCOMING_PDU(PDU_LB_RouteLoungePDU, CpduRouteLoungePdu);


CpduRouteLoungePdu::CpduRouteLoungePdu()
{
  type_ = PDU_LB_RouteLoungePDU;
  length_ = PDU_HEADER_SIZE + 
              sizeof(pdu_routingdata) +
              PDU_STRINGSIZE; // will change
  stuffHeader();
}

// Wrap 'pdu' inside this PDU and send it.
int CpduRouteLoungePdu::routePduByWaitinglist(Cpdu* pdu,
                                              int queueNumber,
                                              const char* username)
{
  char* buf = value_ + PDU_HEADER_SIZE;

  // Append routing data.
  pdu_routingdata data;
  ::ZeroMemory(&data, sizeof(data));
  data.flag = htons(CpduRouteLoungePdu::WaitinglistNumber);
  data.number = htons(queueNumber);
  memcpy(buf, &data, sizeof(data));
  buf += sizeof(data);

  // Append player name
  strncpy(buf, username, PDU_STRINGSIZE);
  buf += PDU_STRINGSIZE;

  if (length_ + pdu->getLength() >= MAXBUFFERSIZE)
  {
    Sys_LogError("CpduRouteLoungePdu: length > MAXBUFFERSIZE (1)");
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


// Wrap 'pdu' inside this PDU and send it.
int CpduRouteLoungePdu::routePduByTable(Cpdu* pdu,
                                        int tableNumber,
                                        const char* username)
{
  // Add table number to the PDU for routing
  // purposes
  char* buf = value_ + PDU_HEADER_SIZE;

  // Append routing data.
  pdu_routingdata data;
  ::ZeroMemory(&data, sizeof(data));
  data.flag = htons(CpduRouteLoungePdu::TableNumber);
  data.number = htons(tableNumber);
  memcpy(buf, &data, sizeof(data));
  buf += sizeof(data);

  // Append player name
  strncpy(buf, username, PDU_STRINGSIZE);
  buf += PDU_STRINGSIZE;

  if (length_ + pdu->getLength() >= MAXBUFFERSIZE)
  {
    Sys_LogError("CpduRouteLoungePdu: length > MAXBUFFERSIZE (2)");
    return -1;
  }

  // Copy the PDU data
  memcpy(buf, pdu->getValue(), pdu->getLength());

  // Adjust PDU length
  length_ += pdu->getLength();
  stuffHeader();

  // Send the PDU to the Load Balancer which will forward
  // it to the appropriate Lounge Server.
  if (CLoadBalancer::Inst())
    return sendTo(CLoadBalancer::Inst()->getSocket());
  else
    return 0;
}


// Extract the wrapped PDU and handle it.
void CpduRouteLoungePdu::grok(SOCKET sd, CLoadBalancer* lb)
{
  int rc = 0;

  pdu_routingdata* data = reinterpret_cast<pdu_routingdata*>(value_ + PDU_HEADERSIZE);

  int flag = ntohs(data->flag);
  int number = ntohs(data->number);
  data++;

  char* buf = reinterpret_cast<char*>(data);
  char sz_username[PDU_STRINGSIZE + 1];
  ::ZeroMemory(sz_username, sizeof(sz_username));
  memcpy(sz_username, buf, PDU_STRINGSIZE);

  // point to the start of the routed PDU data
  // and create a copy of it
  buf += PDU_STRINGSIZE;
  pduh_t* header = reinterpret_cast<pduh_t*>(buf);
  Cpdu* routedPdu = AllocPdu(ntohs(header->type));

  if (routedPdu)
  {
    if (routedPdu->setData(ntohs(header->type),
                           ntohs(header->length),
                           buf))
    {
      CPlayer* player = CLounge::Inst()->addRemotePlayer(sz_username);
      if (player)
      {
        // let the PDU do its magick
        routedPdu->grok(INVALID_SOCKET, player);
        routedPdu = NULL;
      }
    }
  }

  if (routedPdu)
  {
    delete routedPdu;
    routedPdu = NULL;
  }

  delete this;
}
