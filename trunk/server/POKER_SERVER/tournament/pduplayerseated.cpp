/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerSeated
  Created  : 07/10/2000

  OVERVIEW : Send-only player seated pdu.
             Table server tells lounge server that a player was seated.

 ****************************************************************************/

#include "tournament/pduplayerseated.h"

CpduPlayerSeated::CpduPlayerSeated()
{
    type_ = PDU_Tournament_PlayerSeated;
    length_ = PDU_HEADERSIZE + sizeof(pdu_playerseated);
    stuffHeader();
}

int CpduPlayerSeated::sendPlayerSeated(SOCKET      sd,
                                       const char* username)
{
    pdu_playerseated pdu;
    memset(&pdu, 0, sizeof(pdu));

    strncpy(pdu.username, username, PDU_STRINGSIZE);

    memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}
