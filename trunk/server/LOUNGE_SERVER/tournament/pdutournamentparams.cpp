/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentParams
  Created  : 07/08/2000

  OVERVIEW : Send-only tournament parameter pdu.
             Lounge server specifies tournament parameters to
             table server with this pdu.

 ****************************************************************************/

#include "tournament/pdutournamentparams.h"
#include "tournament/basetournamentqueue.h"

#pragma NETWORK_ALIGN_BEGIN
struct pdu_tournamentparams
{
    u_int16_t flag;
    u_int32_t param1;
    u_int32_t param2;
};
#pragma NETWORK_ALIGN_END

CpduTournamentParams::CpduTournamentParams()
{
    type_ = PDU_Tournament_Params;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_tournamentparams);
    stuffHeader();
}

int CpduTournamentParams::sendTournamentParams(CTable* table,
                                               u_int16_t flag,
                                               u_int32_t p1,
                                               u_int32_t p2)
{
    pdu_tournamentparams pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.flag = htons(flag);
    pdu.param1 = htonl(p1);
    pdu.param2 = htonl(p2);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(table->getSocket());
}

int CpduTournamentParams::sendTournamentParams(CBaseTournamentQueue* queue,
                                               u_int16_t flag,
                                               u_int32_t p1,
                                               u_int32_t p2)
{
    int rc = 0;
    pdu_tournamentparams pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.flag = htons(flag);
    pdu.param1 = htonl(p1);
    pdu.param2 = htonl(p2);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    TableList& tl = queue->getTableList();
    for (TableList::iterator tb = tl.begin(),
                             te = tl.end();
         tb != te;
         ++tb)
    {
        rc = sendTo((*tb)->getSocket());
    }

    return rc;
}

