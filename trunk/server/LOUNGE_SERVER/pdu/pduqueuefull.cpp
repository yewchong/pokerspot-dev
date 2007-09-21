/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduQueueFull
  Created  : 05/14/2000

  OVERVIEW : Send-only queue full PDU.

 ****************************************************************************/

#include "pduqueuefull.h"
#include "lounge/player.h"

CpduQueueFull::CpduQueueFull()
{
  type_ = PDU_Lounge_QueueFull;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_queuefull);
  stuffHeader();
}

int CpduQueueFull::sendQueueFull(u_int16_t queuenumber,
                                 CPlayer* player,
                                 u_int16_t reason)
{
  length_ += sizeof(pdu_queuefull);
  stuffHeader();

  char* buf = (value_ + PDU_HEADERSIZE);
  pdu_queuefull data;
  memset(&data, 0, sizeof(data));
  data.queueNumber = htons(queuenumber);
  data.reason = htons(reason);
  memcpy(buf, &data, sizeof(data));

  return sendTo(player);
}

