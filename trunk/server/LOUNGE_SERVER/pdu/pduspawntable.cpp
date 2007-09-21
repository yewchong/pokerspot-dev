/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSpawnTable
  Created  : 04/20/2000

  OVERVIEW : Send-only spawn table PDU - Lounge server
             wants to spawn a table server.

 ****************************************************************************/

#include "pduspawntable.h"

CpduSpawnTable::CpduSpawnTable()
{
    type_ = PDU_Server_SpawnTable;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_spawntable);
    stuffHeader();
}

int CpduSpawnTable::sendSpawnTable(SOCKET sd,
                                   u_int16_t gameType,
                                   u_int16_t tableNumber,
                                   u_int16_t tableLow,
                                   u_int16_t tableHigh)
{
    pdu_spawntable pdu;
    memset(&pdu, 0, sizeof(pdu));
    pdu.gametype = htons(gameType);
    pdu.tablenumber = htons(tableNumber),
    pdu.tablehigh = htons(tableHigh);
    pdu.tablelow = htons(tableLow);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}