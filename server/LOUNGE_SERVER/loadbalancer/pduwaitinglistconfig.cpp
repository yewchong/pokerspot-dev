/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduWaitinglistConfig
  Created  : 10/06/2001

  OVERVIEW : This class is used by the Load Balancer to send
             information about waiting lists to Lounge Servers.

 ****************************************************************************/


#include "loadbalancer/pduwaitinglistconfig.h"
#include "loadbalancer/loadbalancer.h"
#include "sys.h"


REGISTER_INCOMING_PDU(PDU_LB_WaitinglistConfig, CpduWaitinglistConfig);


CpduWaitinglistConfig::CpduWaitinglistConfig()
{}


void CpduWaitinglistConfig::grok(SOCKET sd, CLoadBalancer* lb)
{
  char* buf = value_ + PDU_HEADERSIZE;

  pdu_waitinglistheader* header = reinterpret_cast<pdu_waitinglistheader*>(buf);
  u_int16_t numEntries = ntohs(header->numEntries);
  u_int16_t op = ntohs(header->operation);
  buf += sizeof(pdu_waitinglistheader);

  for (int i = 0; i < numEntries; i++)
  {
    switch (op)
    {
    case Add:
      break;

    case Remove:
      break;

    case SetOwner:
      {
        pdu_waitinglistowner* data = reinterpret_cast<pdu_waitinglistowner*>(buf);
        buf += sizeof(pdu_waitinglistowner);

        lb->setWaitinglistOwner(ntohs(data->number),
                                ntohs(data->owner));           
      }
      break;

    default:
      Sys_LogError("Unknown case CpduWaitinglistConfig::grok");
      break;
    }
  }

  delete this;
}