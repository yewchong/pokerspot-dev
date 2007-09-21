/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionRequestSpreadLimit
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to request
             for an action in a no-limit game. The spotlight player will
             respond with an action PDU.

 ****************************************************************************/

#include "pduactionrequestspreadlimit.h"
#include "table/table.h"
#include "player/player.h"
#include "common/chips.h"

#pragma NETWORK_ALIGN_BEGIN
struct spreadlimit_pdu
{
  u_int16_t slot;                // which player is this request for?
  CChips::chip_amount_t value;   // amount to pay
  CChips::chip_amount_t raiseLo; // lo limit raise, 0: no raise allowed
  CChips::chip_amount_t raiseHi; // lo limit raise, 0: no raise allowed
  u_int16_t msecs;               // time allowed for thinking
  u_int16_t sequencenumber;
};
#pragma NETWORK_ALIGN_END


CpduActionRequestSpreadLimit::CpduActionRequestSpreadLimit(CTable* table)
  :
  table_(table)
{
  type_ = PDU_Play_ActionRequestSpreadLimit;
  length_ = PDU_HEADER_SIZE + sizeof(spreadlimit_pdu);
  stuffHeader();
}

int CpduActionRequestSpreadLimit::sendActionRequest(
                                    CPlayer* player,
                                    const CChips& toPay,
                                    const CChips& raiseLo,
                                    const CChips& raiseHi,
                                    u_int16_t responseTime,
                                    u_int16_t sequenceNumber)
{
  spreadlimit_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.slot = htons(player->getSlot());
  pdu.value = toPay.htonl_rep();
  pdu.raiseLo = raiseLo.htonl_rep();
  pdu.raiseHi = raiseHi.htonl_rep();
  pdu.msecs = htons(responseTime);
  pdu.sequencenumber = htons(sequenceNumber);

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return broadcastTo(table_->getRing());
}
