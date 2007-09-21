/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentDetailsReply
  Created  : 07/07/2000

  OVERVIEW : Send-only tournament details reply PDU.

 ****************************************************************************/

#include "tournament/pdutournamentdetailsreply.h"
#include "tournament/tournament.h"
#include "lounge/player.h"
#include "common/chips.h"

#pragma NETWORK_ALIGN_BEGIN
struct pdu_tournamentdetailsreply
{
  CChips::chip_amount_t buyin;
  u_int16_t numplayers;
  u_int32_t starttime;
};
#pragma NETWORK_ALIGN_END


CpduTournamentDetailsReply::CpduTournamentDetailsReply()
{
    type_ = PDU_Tournament_DetailsReply;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_tournamentdetailsreply);
    stuffHeader();
}


int CpduTournamentDetailsReply::sendReply(CTournament* t,
                                          CPlayer* p)
{
    pdu_tournamentdetailsreply pdu;
    memset(&pdu, 0, sizeof(pdu));
    pdu.buyin = t->getBuyin().htonl_rep();
    pdu.numplayers = htons(t->getNumPlayersPlaying());

    if (t->getStartTime() == 0)
    { // This is a satellite tournament, starts when
      // enough players in the waiting list - use
      // starttime field to pass the number of players.
      pdu.starttime = htonl(t->getMaxPlayers());
    }
    else
    {
      pdu.starttime = htonl(t->getStartTime());
    }

    char* buf = (value_ + PDU_HEADER_SIZE);
    memcpy(buf, &pdu, sizeof(pdu));

    string descr = t->getDescription();

    if ((length_ + descr.size() + 1) < MAXBUFFERSIZE)
    {
        length_ += descr.size() + 1;
        buf += sizeof(pdu_tournamentdetailsreply);

        if (descr.size() > 0)
        {
            memcpy(buf, descr.c_str(), descr.size());
            buf += descr.size();
            *buf = '\0';
        }
    }

    stuffHeader();

    return sendTo(p->getSocket());
}
