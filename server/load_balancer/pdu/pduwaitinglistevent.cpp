/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduWaitinglistEvent
  Created  : 10/02/2001

  OVERVIEW : This PDU is used for sending waiting list events to
             other Lounge Servers.

             The possible waiting list events are:
             - table addition
             - table remove
             - tournament addition
             - tournament remove

 ****************************************************************************/

#include "pdu/pduwaitinglistevent.h"
#include "loadbalancer/loadbalancer.h"
#include "loadbalancer/lounge.h"
#include "common/errorlog.h"


REGISTER_INCOMING_PDU(PDU_LB_WaitinglistEvent, CpduWaitinglistEvent);


CpduWaitinglistEvent::CpduWaitinglistEvent()
{
  type_ = PDU_LB_WaitinglistEvent;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}


void CpduWaitinglistEvent::grok(SOCKET sd, CLounge* lounge)
{
  // Broadcast the message to all other lounges
  // than the one that sent the message.
  broadcast(lounge->getSocket());

  delete this;
}
