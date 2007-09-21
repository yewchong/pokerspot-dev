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
#include "lounge/loungeserver.h"
#include "table/table.h"


REGISTER_INCOMING_PDU(PDU_Tournament_ReseatPlayer, CpduReseatPlayer);

struct pdu_reseatplayer
{
    char username[PDU_STRINGSIZE];
    u_int32_t addr;
    u_int16_t port;
};

CpduReseatPlayer::CpduReseatPlayer()
{}

void CpduReseatPlayer::grok(SOCKET sd, CTable* table)
{
    pdu_reseatplayer* pdu = (pdu_reseatplayer*)(value_ + PDU_HEADER_SIZE);

    char sz_username[17];
    memset(sz_username, 0, sizeof(sz_username));
    strncpy(sz_username, pdu->username, PDU_STRINGSIZE);
    printf("CpduReseatPlayer: %s\n", sz_username);

    table->reseatPlayer(sz_username,
                        ntohl(pdu->addr),
                        ntohs(pdu->port));

    char buf[100];
    sprintf(buf, "Player %s reseated", sz_username);
    Sys_LogTournament(buf);

    CLoungeServer::Inst()->sendPlayerUnseated(sz_username);

    delete this;
}
