/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBuyinReply
  Created  : 04/20/2000

  OVERVIEW : Send-only buy in reply PDU.

 ****************************************************************************/

#include "pdubuyinreply.h"
#include "lounge/player.h"

CpduBuyinReply::CpduBuyinReply()
{
    type_ = PDU_Lounge_BuyInReply;
    length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
    stuffHeader();
}

int CpduBuyinReply::sendBuyinReply(CPlayer* player, u_int16_t indicator)
{
    char* buf = value_ + PDU_HEADER_SIZE;
    indicator = htons(indicator);
    memcpy(buf, &indicator, sizeof(indicator));
    return sendTo(player->getSocket());
}
