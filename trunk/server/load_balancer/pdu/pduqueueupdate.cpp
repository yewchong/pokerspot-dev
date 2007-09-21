
/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduQueueUpdate
  Created  : 10/08/2001

  OVERVIEW : This PDU is used for broadcasting changes
             in the waiting lists.

 ****************************************************************************/


#include "pdu/pduqueueupdate.h"


REGISTER_INCOMING_PDU(PDU_Lounge_QueueUpdate, CpduQueueUpdate);


CpduQueueUpdate::CpduQueueUpdate()
{}


void CpduQueueUpdate::grok(SOCKET, CLounge*)
{
  // Broadcast the PDU to all lounges.
  broadcast();

  delete this;
}
