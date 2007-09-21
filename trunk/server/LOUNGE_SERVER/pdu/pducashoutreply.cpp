/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCashoutReply
  Created  : 04/20/2000

  OVERVIEW : Send-only cash out reply PDU.

 ****************************************************************************/

#include "pducashoutreply.h"
#include "lounge/player.h"

CpduCashoutReply::CpduCashoutReply()
{
    type_ = PDU_Lounge_CashOutReply;
    length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
    stuffHeader();
}

int CpduCashoutReply::sendCashoutReply(CPlayer* player, u_int16_t indicator)
{
    char* buf = value_ + PDU_HEADER_SIZE;
    indicator = htons(indicator);
    memcpy(buf, &indicator, sizeof(indicator));
    return sendTo(player->getSocket());
}


