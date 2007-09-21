/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSynchronize
  Created  : 07/14/2000

  OVERVIEW : This PDU is used for synchronizing hands in tournaments.
             Lounge server sends this pdu to tables to ensure hands
             run synchronized. 

 ****************************************************************************/

#include "tournament/pdusynchronize.h"
#include "tournament/tournament.h"

REGISTER_INCOMING_PDU(PDU_Tournament_Synchronize, CpduSynchronize);


struct pdu_sync
{
    u_int16_t flag;
};


CpduSynchronize::CpduSynchronize()
{
    type_ = PDU_Tournament_Synchronize;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_sync);
    stuffHeader();
}


int CpduSynchronize::sendSync(SOCKET sd, SyncFlag sf)
{
    pdu_sync pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.flag = htons((u_int16_t)sf);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}

void CpduSynchronize::grok(SOCKET, CTable* table)
{
    pdu_sync* pdu = (pdu_sync*)(value_ + PDU_HEADER_SIZE);

    SyncFlag sf = (SyncFlag)ntohs(pdu->flag);
    CTournament::Inst()->setSyncState(sf);

    printf("TOURNAMENT: CpduSynchronize %d\n", sf);

    if (sf == SF_Play)
        CTournament::Inst()->setPauseTime(0);

    delete this;
}
