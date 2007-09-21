#ifndef __lounge_pdutournamentdetailsquery_h__
#define __lounge_pdutournamentdetailsquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentDetailsQuery
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament details query PDU.

 ****************************************************************************/

#include "pdu/pdu.h"

// This comes from the player
struct pdu_tournamentdetailsquery
{
    u_int16_t number;
};

class CpduTournamentDetailsQuery : public Cpdu
{
public:
    CpduTournamentDetailsQuery();
    void grok(SOCKET sd, CPlayer*);
    u_int16_t getTournamentNumber() const;
};

inline u_int16_t CpduTournamentDetailsQuery::getTournamentNumber() const
{
    pdu_tournamentdetailsquery* pdu = (pdu_tournamentdetailsquery*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->number);
}


#endif