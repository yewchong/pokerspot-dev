/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginQuery
  Created  : 04/24/2000

  OVERVIEW : Send-only Player Login Query PDU - lounge server sends
             this pdu to a table server to ask if a player can be
             seated in that table.

 ****************************************************************************/

#include "pduplayerloginquery.h"
#include "lounge/table.h"
#include "lounge/player.h"

struct pdu_playerloginquery
{
  u_int32_t ipaddress;
  char username[PDU_STRINGSIZE];
};


CpduPlayerLoginQuery::CpduPlayerLoginQuery()
{
  type_ = PDU_Server_PlayerLoginQuery;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_playerloginquery);
  stuffHeader();
}

int CpduPlayerLoginQuery::sendLoginQuery(CTable* table,
                                         CPlayer* player)
{
  pdu_playerloginquery pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.ipaddress = htonl(player->getIpAddress());

  const char* username = player->getUsername();
  if (username != NULL && *username != '\0')
    strncpy(pdu.username, player->getUsername(), PDU_STRINGSIZE);

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return sendTo(table->getSocket());
}

