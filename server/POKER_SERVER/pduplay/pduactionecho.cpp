/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionEcho
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to echo
             an action of one client.

 ****************************************************************************/

#include "pduactionecho.h"
#include "table/table.h"
#include "player/player.h"

struct actionecho_pdu
{
  u_int16_t slot;               // which player?
  u_int16_t action;             // 1:fold 2:call 3:raise 4:post blind
  CChips::chip_amount_t amount; // amount called/raised/posted
};


CpduActionEcho::CpduActionEcho(CTable* table)
  :
  table_(table)
{
  type_ = PDU_Play_ActionEcho;
  length_ = PDU_HEADER_SIZE + sizeof(actionecho_pdu);
  stuffHeader();
}

int CpduActionEcho::sendActionEcho(CPlayer* player,
                                   u_int16_t action,
                                   const CChips& amount)
{
  actionecho_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));
  pdu.slot = htons(player->getSlot());
  pdu.action = htons(action);
  pdu.amount = amount.htonl_rep();

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return broadcastTo(table_->getRing());
}

