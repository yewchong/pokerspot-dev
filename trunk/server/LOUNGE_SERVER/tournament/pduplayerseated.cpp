/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerSeated
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament player seated pdu.
             Table server tells lounge server that a player
             has been seated at the table.

 ****************************************************************************/

#include "tournament/pduplayerseated.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "lounge/lounge.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Tournament_PlayerSeated, CpduPlayerSeated);


CpduPlayerSeated::CpduPlayerSeated()
{
    memset(sz_username, 0, sizeof(sz_username));
}

void CpduPlayerSeated::grok(SOCKET sd, CTable* table)
{
  // Copy username zero-terminated string
	pdu_playerseated* data = (pdu_playerseated*)(value_ + PDU_HEADERSIZE);
  memcpy(sz_username, data->username, PDU_STRINGSIZE);

  // Forward the event to tournament
  CBaseTournamentQueue* tq = 
    CBaseTournamentQueue::FindQueue(table);
  if (tq)
  {
    CTournament* t = tq->findTournamentByTable(table->getNumber());
    if (t)
      t->playerReseat(table, sz_username);
  }

  // Update the player list in the table
  CLounge::Inst()->playerSeated(sz_username, table);

  // this pdu is done
  delete this;
}
