/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSitting
  Created  : 04/23/2000

  OVERVIEW : Send-only sitting PDU; this PDU tells the client that
             the player has been taken from the waiting list and
             seated in a table.

 ****************************************************************************/

#include "pdusitting.h"
#include "lounge/lounge.h"


struct pdu_sitting
{
  u_int16_t table;
  u_int16_t port;
  u_int32_t host;
};


CpduSitting::CpduSitting()
{
  type_ = PDU_Lounge_Sitting;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_sitting);
  stuffHeader();
}

int CpduSitting::sendSitting(CPlayer*  player,
                             u_int16_t table,
                             u_int16_t port,
                             u_int32_t host)
{
  pdu_sitting pdu;
  memset(&pdu, 0, sizeof(pdu));
  
  pdu.table = htons(table);
  pdu.port = htons(port);
  pdu.host = htonl(host);
  
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
  
  return player->send(*this);
}

