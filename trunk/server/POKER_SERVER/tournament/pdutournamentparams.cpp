/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentParams
  Created  : 07/08/2000

  OVERVIEW : Tournament parameters pdu.
             Lounge server specifies tournament parameters to
             table server with this pdu.
             The table server forwards the pdu to the client
             so as to inform it about the change.

 ****************************************************************************/

#include "pdutournamentparams.h"
#include "tournament.h"
#include "table/table.h"
#include "pdunotify/pdunotify.h"

REGISTER_INCOMING_PDU(PDU_Tournament_Params, CpduTournamentParams);

CpduTournamentParams::CpduTournamentParams()
{
    type_ = PDU_Tournament_Params;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_tournamentparams);
    stuffHeader();
}

void CpduTournamentParams::grok(SOCKET sd, CTable* table)
{
    ASSERT(CTournament::Inst());

    pdu_tournamentparams* pdu = (pdu_tournamentparams*)(value_ + PDU_HEADER_SIZE);

    u_int16_t flag = ntohs(pdu->flags);
    u_int32_t p1 = ntohl(pdu->param1);
    u_int32_t p2 = ntohl(pdu->param2);

    printf("CpduTournamentParams %d %d %d\n",
           flag, p1, p2);

    CTournament* t = CTournament::Inst();

    if (flag == TF_Start)
    {
        // Everybody starts with no missed blinds
        table->resetMissedBlinds();

        // echo this pdu to the clients
        sendParams(table, TF_Start, p1, p2);
    }
    else if (flag == TF_Number)
    {
        t->isTournament(true);
        t->setTournamentNumber(p1);
        t->setNumPlayers(p2);
        printf("Tournament %i players %i\n", p1, p2);
    }
    else if (flag == TF_End)
    {
        CpduNotify pdu;
        pdu.sendObservers(table, "This table is dissolved.");
        table->setShutdown(10, false); // no shutdown log
    }
    else if (flag == TF_GameType)
    {
        // set new game type
        printf("NEW GAME TYPE: %d %d\n", p1, p2);
        CTournament::Inst()->setGameType(p1);
        CTournament::Inst()->setIsHiLo((p2 != 0) ? true : false);
    }
    else if (flag == TF_BetStructure)
    {
        CTournament::Inst()->setBettingStructure(p1);
    }
    else if (flag == TF_Limit)
    {
        CChips lo, hi;
        lo.setRep(p1);
        hi.setRep(p2);

        CTournament::Inst()->setLimits(lo, hi);
    }
    else if (flag == TF_FillSeats)
    {
        CChips chips;
        chips.setRep(p1);
        table->fillSeats(chips);
    }
    else if (flag == TF_Freeze)
    {
        CTournament::Inst()->setIsFrozen(true);
    }
    else if (flag == TF_Winner)
    {
        CTournament::Inst()->declareWinner(table);
    }
    else if (flag == TF_Pause)
    {
        CTournament::Inst()->setPauseTime(p1);
    }

    delete this;
}


int CpduTournamentParams::sendParams(CPlayer* player,
                                     u_int16_t flag,
                                     u_int32_t p1,
                                     u_int32_t p2)
{
    pdu_tournamentparams pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.flags = htons(flag);
    pdu.param1 = htonl(p1);
    pdu.param2 = htonl(p2);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
    
    return sendTo(player->getConnfd());
}


int CpduTournamentParams::sendParams(CTable*   table,
                                     u_int16_t flag,
                                     u_int32_t p1,
                                     u_int32_t p2)
{
    pdu_tournamentparams pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.flags = htons(flag);
    pdu.param1 = htonl(p1);
    pdu.param2 = htonl(p2);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
    
    return broadcastTo(table->getRing());
}