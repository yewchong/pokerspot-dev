/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerSeated
  Created  : 07/10/2000

  OVERVIEW : Send-only player unseated pdu.
             Table server tells lounge server that a player was logged out.

 ****************************************************************************/

#include "tournament/pduplayerunseated.h"
#include "tournament/pduplayerseated.h"

CpduPlayerUnseated::CpduPlayerUnseated()
{
    type_ = PDU_Tournament_PlayerUnseated;
    length_ = PDU_HEADERSIZE + sizeof(pdu_playerseated);
    stuffHeader();
}

int CpduPlayerUnseated::sendPlayerUnseated(SOCKET      sd,
                                           const char* username)
{
    pdu_playerseated pdu;
    memset(&pdu, 0, sizeof(pdu));

    strncpy(pdu.username, username, PDU_STRINGSIZE);

    memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}
