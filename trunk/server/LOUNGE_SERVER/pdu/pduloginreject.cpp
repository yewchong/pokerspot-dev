/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoginReject
  Created  : 04/18/2000

  OVERVIEW : Login reject pdu.

 ****************************************************************************/

#include "pduloginreject.h"
#include "lounge/player.h"

CpduLoginReject::CpduLoginReject()
{
    type_ = PDU_Lounge_Reject;
    length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
    stuffHeader();
}

int CpduLoginReject::sendReject(SOCKET sd, RejectFlag rejectFlag)
{
    u_int16_t flag = htons(rejectFlag);
    memcpy(value_ + PDU_HEADER_SIZE, &flag, sizeof(flag));
    return sendTo(sd);
}