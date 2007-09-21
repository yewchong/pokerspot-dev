/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTournamentDetailsQuery
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament details query PDU.

 ****************************************************************************/

#include "tournament/pdutournamentdetailsquery.h"
#include "tournament/pdutournamentdetailsreply.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Tournament_DetailsQuery, CpduTournamentDetailsQuery);

CpduTournamentDetailsQuery::CpduTournamentDetailsQuery()
{}

void CpduTournamentDetailsQuery::grok(SOCKET sd, CPlayer* player)
{
  u_int16_t number = getTournamentNumber();

  CBaseTournamentQueue*
    tq = CBaseTournamentQueue::FindQueueByTournamentNumber(number);
  if (tq)
  {
    CTournament* t = tq->findTournament(number);
    if (t)
    {
      CpduTournamentDetailsReply pdu;
      pdu.sendReply(t, player);
    }
  }

  // This pdu is done
  delete this;
}