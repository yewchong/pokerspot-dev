/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionRequest
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to request
             for an action. The spotlight player will respond with an
             action PDU.

 ****************************************************************************/



#include "pduactionrequest.h"
#include "table/table.h"
#include "player/player.h"
#include "common/chips.h"

#pragma NETWORK_ALIGN_BEGIN
struct actionrequest_pdu
{
  u_int16_t slot;              // which player is this request for?
  CChips::chip_amount_t value; // amount to pay
  CChips::chip_amount_t raise; // amount to raise, 0: no raise allowed
  u_int16_t msecs;             // time allowed for thinking
  u_int16_t sequencenumber;
};
#pragma NETWORK_ALIGN_END

CpduActionRequest::CpduActionRequest(CTable* table)
  :
  table_(table)
{
  type_ = PDU_Play_ActionRequest;
  length_ = PDU_HEADER_SIZE + sizeof(actionrequest_pdu);
  stuffHeader();
}

int CpduActionRequest::sendActionRequest(CPlayer* player,
                                         const CChips& toPay,
                                         const CChips& raise,
                                         u_int16_t responseTime,
                                         u_int16_t sequenceNumber)
{
  actionrequest_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));
  pdu.slot = htons(player->getSlot());
  pdu.value = toPay.htonl_rep();
  pdu.raise = raise.htonl_rep();
  pdu.msecs = htons(responseTime);
  pdu.sequencenumber = htons(sequenceNumber);

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return broadcastTo(table_->getRing());
}
