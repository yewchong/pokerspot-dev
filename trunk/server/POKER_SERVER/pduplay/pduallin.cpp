/*******************************************************************************

    NAME:       pduAllIn
    DATE:       4/12/00
    PURPOSE:    Class CpduAllIn implements the send-only PDU type
                PDU_Play_AllIn.  This is the PDU sent to all players
                to inform them that a player has gone "all-in".

*******************************************************************************/

#include "pduallin.h"

CpduAllIn::CpduAllIn(CTable* table)
  :
  table_(table)
{
  type_ = PDU_Play_AllIn;
  length_ = PDU_HEADER_SIZE + sizeof(allin_pdu);
  stuffHeader();
}

int CpduAllIn::sendAllIn(CPlayer* player, const CChips& toPay)
{
  allin_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));
  pdu.slot = (u_int16_t) htons(player->getSlot());
  pdu.topay = toPay.htonl_rep();

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return broadcastTo(table_->getRing());
}