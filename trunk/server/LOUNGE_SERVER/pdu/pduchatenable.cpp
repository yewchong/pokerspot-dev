/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBuyinReply
  Created  : 04/20/2000

  OVERVIEW : Send-only buy in reply PDU.

 ****************************************************************************/

#include "pduchatenable.h"

CpduChatEnable::CpduChatEnable()
{
    type_ = PDU_ChatEnable;
    length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
    stuffHeader();
}

int CpduChatEnable::sendChatEnable(SOCKET sd,
                                   u_int16_t flags,
                                   const char* message)
{
    char* buf = value_ + PDU_HEADER_SIZE;
    flags = htons(flags);
    memcpy(buf, &flags , sizeof(flags));
    return sendTo(sd);
}

int CpduChatEnable::broadcastChatEnable(u_int16_t flags,
                                        const char* message)
{
    char* buf = value_ + PDU_HEADER_SIZE;
    flags = htons(flags);
    memcpy(buf, &flags , sizeof(flags));
    return Cpdu::broadcastToPlayers();
}

