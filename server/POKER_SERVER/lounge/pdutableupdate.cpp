/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableUpdate
  Created  : 04/24/2000

  OVERVIEW : Send-only Table Update PDU - table server sends
             this pdu to inform lounge server about state change.

 ****************************************************************************/

#include "pdutableupdate.h"

CpduTableUpdate::CpduTableUpdate()
{
    type_ = PDU_Server_TableUpdate;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_tableupdate);
    stuffHeader();
}

int CpduTableUpdate::sendTableUpdate(SOCKET sd, u_int16_t numPlayers)
{
    pdu_tableupdate pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.numplayers = htons(numPlayers);
    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}
