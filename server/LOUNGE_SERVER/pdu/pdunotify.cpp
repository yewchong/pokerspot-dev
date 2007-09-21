/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduNotify
  Created  : 04/19/2000

  OVERVIEW : Send-only Notify pdu.

 ****************************************************************************/

#include "pdunotify.h"
#include "lounge/player.h"
#include "lounge/lounge.h"

CpduNotify::CpduNotify()
{
    type_ = PDU_Server_Notify;
    length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
    stuffHeader();
}

int CpduNotify::sendNotifyMessage(SOCKET sd,
                                  const char* msg,
                                  int flags)
{
    char* buf = value_ + PDU_HEADER_SIZE;
    u_int16_t f = htons(flags);
    memcpy(buf, &f, sizeof(f));
    buf += sizeof(f);

    if (msg != NULL && *msg != '\0')
    {
        size_t length = strlen(msg);
        memcpy(buf, msg, length);
        buf[length] = '\0';
        length_ += (length + 1);
    }

    stuffHeader();

    return sendTo(sd);
}

// XXX
int CpduNotify::sendAllNotifyMessage(const char* msg, int flags)
{
    char* buf = value_ + PDU_HEADER_SIZE;
    u_int16_t f = htons(flags);
    memcpy(buf, &f, sizeof(f));
    buf += sizeof(f);

    if (msg != NULL && *msg != '\0')
    {
        size_t length = strlen(msg);
        memcpy(buf, msg, length);
        buf[length] = '\0';
        length_ += (length + 1);
    }

    stuffHeader();
    int rc = 0;

    for (PlayerIterator p = CLounge::Inst()->allPlayers();
         p.more(); p.next())
    {
        CPlayer* player = p.current();
        SOCKET sd = player->getSocket();
        if (sd != -1)
            rc = sendTo(sd);
    }

    return rc;
}