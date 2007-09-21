/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduBlindRequest
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to prompt
             for a player to post the blind.

 ****************************************************************************/

#include "pdublindrequest.h"
#include "table/table.h"
#include "player/player.h"


#pragma NETWORK_ALIGN_BEGIN
struct blindrequest_pdu
{
  u_int16_t slot;              // which player is this request for?
  CChips::chip_amount_t value; // amount to post
  u_int16_t msecs;             // time to think
  u_int16_t sequencenumber;
};
#pragma NETWORK_ALIGN_END


CpduBlindRequest::CpduBlindRequest(CTable* table)
  :
  table_(table)
{
  type_ = PDU_Play_BlindRequest;
  length_ = PDU_HEADER_SIZE + sizeof(blindrequest_pdu);
  stuffHeader();
}

int CpduBlindRequest::sendBlindRequest(CPlayer* player,
                                       const CChips& value,
                                       u_int16_t responseTime,
                                       u_int16_t sequenceNumber)
{
  blindrequest_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));
  pdu.slot = htons(player->getSlot());
  pdu.value = value.htonl_rep();
  pdu.msecs = htons(responseTime);
  pdu.sequencenumber = htons(sequenceNumber);

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return broadcastTo(table_->getRing());
}
