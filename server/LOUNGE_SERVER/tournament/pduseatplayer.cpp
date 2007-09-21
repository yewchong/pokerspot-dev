/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSeatPlayer
  Created  : 07/07/2000

  OVERVIEW : Send-only tournament seat player pdu.
             Lounge server tells table server to seat a player
             at the table.

 ****************************************************************************/

#include "tournament/pduseatplayer.h"
#include "lounge/table.h"
#include "common/chips.h"

struct pdu_seatplayer
{
    char username[PDU_STRINGSIZE];
    CChips::chip_amount_t chips;
    u_int16_t flags;
};

CpduSeatPlayer::CpduSeatPlayer()
{
    type_ = PDU_Tournament_SeatPlayer;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_seatplayer);
    stuffHeader();
}

int CpduSeatPlayer::sendSeatPlayer(const char*   username,
                                   const CChips& chips,
                                   u_int16_t     flags,
                                   CTable*       table)
{
    pdu_seatplayer pdu;
    memset(&pdu, 0, sizeof(pdu));

    strncpy(pdu.username, username, PDU_STRINGSIZE);
    pdu.chips = chips.htonl_rep();
    pdu.flags = htons(flags);

    memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));
    stuffHeader();

    return sendTo(table->getSocket());
}
