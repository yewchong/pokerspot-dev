/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduClientReseat
  Created  : 07/26/2000

  OVERVIEW : The table server sends this PDU to the player to
             reseat him at another table.

 ****************************************************************************/

#include "tournament/pduclientreseat.h"


CpduClientReseat::CpduClientReseat()
{
    type_ = PDU_Tournament_ReseatPlayer;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_reseat);
    stuffHeader();
}

int CpduClientReseat::sendReseat(SOCKET sd, u_int32_t addr, u_int16_t port)
{
    pdu_reseat pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.addr = htonl(addr);
    pdu.port = htons(port);

    memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}

