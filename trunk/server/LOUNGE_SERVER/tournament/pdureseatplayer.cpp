/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduReseatPlayer
  Created  : 07/07/2000

  OVERVIEW : Send-only tournament reseat player pdu.
             Lounge server tells the (old) table server to seat
             a player at a new table. The old table server forwards
             this to the client which closes its connection and
             connects to the new table server.

 ****************************************************************************/


#include "tournament/pdureseatplayer.h"
#include "lounge/table.h"

struct pdu_reseatplayer
{
    char username[PDU_STRINGSIZE];
    u_int32_t addr;
    u_int16_t port;
};

CpduReseatPlayer::CpduReseatPlayer()
{
    type_ = PDU_Tournament_ReseatPlayer;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_reseatplayer);
    stuffHeader();
}

int CpduReseatPlayer::sendReseatPlayer(const char* username,
                                       CTable*     newTable,
                                       CTable*     oldTable)
{
    pdu_reseatplayer pdu;
    memset(&pdu, 0, sizeof(pdu));

    strncpy(pdu.username, username, PDU_STRINGSIZE);
    pdu.addr = htonl(newTable->getAddr());
    pdu.port = htons(newTable->getPort());

    memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));
    stuffHeader();

    return sendTo(oldTable->getSocket());
}
