/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerUnseated
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament player unseated pdu.
             Table server tells lounge server that a player
             has been logged off from the table.

 ****************************************************************************/

#include "tournament/pduplayerunseated.h"
#include "tournament/basetournamentqueue.h"
#include "lounge/lounge.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Tournament_PlayerUnseated, CpduPlayerUnseated);


CpduPlayerUnseated::CpduPlayerUnseated()
{
  memset(sz_username, 0, sizeof(sz_username));
}

void CpduPlayerUnseated::grok(SOCKET sd, CTable* table)
{
  // Copy username to zero-terminated string
	pdu_playerseated* data = (pdu_playerseated*)(value_ + PDU_HEADERSIZE);
  memcpy(sz_username, data->username, PDU_STRINGSIZE);

  // Update the player list in the table
  CLounge::Inst()->playerUnseated(sz_username, table);

  CBaseTournamentQueue* tq =
    CBaseTournamentQueue::FindQueue(table);
  if (tq)
  {
    tq->playerUnseated(sz_username, table);
  }

  // this pdu is done
  delete this;
}
