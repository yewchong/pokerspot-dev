/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduNotify
  Created  : 04/19/2000

  OVERVIEW : Send-only Notify pdu.

 ****************************************************************************/

#include "pdunotify.h"
#include "../player/player.h"
#include "../table/table.h"

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

int CpduNotify::sendAllRaw(CTable* table, const char* msg, int flag)
{
    int rc = 0;
    char* buf = value_ + PDU_HEADER_SIZE;
    u_int16_t f = htons(flag);
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

    int i = 0;
    for (i = 0; i < CTable::MaxPlayers; i++)
    {
        CPlayer* player = table->getPlayerRawSlot(i);
        if (player->getConnfd() != -1)
        {
            rc = sendTo(player->getConnfd());
        }
    }

    // Send also to observers
    for (i = 0; i < CTable::MaxObservers; i++)
    {
        CPlayer* p = table->getObserver(i);
        if (p && (p->getConnfd() != -1))
            rc = sendTo(p->getConnfd());
    }


    return rc;
}

int CpduNotify::sendObservers(CTable* table, const char* msg, int flag)
{
    int rc = 0;
    char* buf = value_ + PDU_HEADER_SIZE;
    u_int16_t f = htons(flag);
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

    // Send to observers
    for (int i = 0; i < CTable::MaxObservers; i++)
    {
        CPlayer* p = table->getObserver(i);
        if (p && (p->getConnfd() != -1))
            rc = sendTo(p->getConnfd());
    }

    return rc;
}
