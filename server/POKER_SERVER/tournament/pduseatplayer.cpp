/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSeatPlayer
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament seat player pdu.
             Lounge server tells table server to seat a player
             at the table.

 ****************************************************************************/

#include "tournament/pduseatplayer.h"
#include "table/table.h"
#include "lounge/loungeserver.h"

REGISTER_INCOMING_PDU(PDU_Tournament_SeatPlayer, CpduSeatPlayer);

struct pdu_seatplayer
{
    char username[PDU_STRINGSIZE];
    CChips::chip_amount_t chips;
    u_int16_t flags;
};

CpduSeatPlayer::CpduSeatPlayer()
{}

void CpduSeatPlayer::grok(SOCKET sd, CTable* table)
{
    pdu_seatplayer* pdu = (pdu_seatplayer*)(value_ + PDU_HEADER_SIZE);

    char sz_username[17];
    memset(sz_username, 0, sizeof(sz_username));
    strncpy(sz_username, pdu->username, PDU_STRINGSIZE);

    u_int16_t flags = ntohs(pdu->flags);

    if (table->seatPlayer(sz_username,
                          CChips_n(pdu->chips).ntohl(),
                          (flags & SF_CheckZombie) ? true : false))
    {
        char buf[100];
        sprintf(buf, "Player %s seated", sz_username);
        Sys_LogTournament(buf);

        if (flags & SF_SendAck)
        {
            CLoungeServer::Inst()->sendReseatAck(sz_username);
        }
    }
    else if (flags & SF_SendAck)
    {
        CLoungeServer::Inst()->sendReseatNack(sz_username);
    }

    delete this;
}
