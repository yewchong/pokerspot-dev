/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSynchronize
  Created  : 07/14/2000

  OVERVIEW : This PDU is used for synchronizing hands in tournaments.
             Lounge server sends this pdu to tables to ensure hands
             run synchronized. Table server sends this pdu with flag
             SF_HandEnd to notify lounge that a hand has ended.

 ****************************************************************************/

#include "tournament/pdusynchronize.h"
#include "tournament/basetournamentqueue.h"
#include "lounge/table.h"

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


int CpduSynchronize::sendSync(CTable*  table,
                              SyncFlag flag)
{
    pdu_sync pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.flag = htons((u_int16_t)flag);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(table->getSocket());
}

int CpduSynchronize::sendSync(CBaseTournamentQueue* tq,
                              SyncFlag flag)
{
    int rc = 0;

    pdu_sync pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.flag = htons((u_int16_t)flag);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    TableList& tl = tq->getTableList();
    for (TableList::iterator tb = tl.begin(),
                             te = tl.end();
         tb != te;
         ++tb)
    {
        rc = sendTo((*tb)->getSocket());
    }

    return rc;
}

void CpduSynchronize::grok(SOCKET, CTable* table)
{
    pdu_sync* pdu = (pdu_sync*)(value_ + PDU_HEADER_SIZE);

    u_int16_t state = ntohs(pdu->flag);
    if (state == SF_Paused || state == SF_Pause)
    {
        printf("%s paused\n", table->getTitle().c_str());
        table->setTournamentState(CTable::TS_Paused);
    }
    else if (state == SF_Play)
    {
        printf("%s running\n", table->getTitle().c_str());
        table->setTournamentState(CTable::TS_Running);
    }

    delete this;
}
