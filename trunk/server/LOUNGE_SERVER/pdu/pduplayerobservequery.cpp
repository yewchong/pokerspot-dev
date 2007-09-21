/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveQuery
  Created  : 04/25/2000

  OVERVIEW : Send-only Player Observe Query PDU - lounge server sends
             this pdu to a table server to ask if a player can observe
             a table.

 ****************************************************************************/

#include "pduplayerobservequery.h"
#include "lounge/table.h"
#include "lounge/player.h"


struct pdu_playerobservequery
{
  u_int32_t ipaddress;
  char username[PDU_STRINGSIZE];
};


CpduPlayerObserveQuery::CpduPlayerObserveQuery()
{
  type_ = PDU_Server_PlayerObserveQuery;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_playerobservequery);
  stuffHeader();
}

int CpduPlayerObserveQuery::sendObserveQuery(CTable* table,
                                             CPlayer* player)
{
  pdu_playerobservequery pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.ipaddress = htonl(player->getIpAddress());

  const char* username = player->getUsername();
  if (username != NULL && *username != '\0')
    strncpy(pdu.username, player->getUsername(), PDU_STRINGSIZE);

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return sendTo(table->getSocket());
}
