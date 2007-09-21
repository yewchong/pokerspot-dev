/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduReseatNack
  Created  : 07/26/2000

  OVERVIEW : Receive-only tournament reseat negative ack pdu.
             Table server tells lounge server that a player
             reseat failed.

 ****************************************************************************/

#include "tournament/pdureseatnack.h"
#include "tournament/pdureseatack.h"
#include "tournament/basetournamentqueue.h"
#include "lounge/lounge.h"
#include "pdu/pduqueuefull.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Tournament_ReseatNack, CpduReseatNack);


CpduReseatNack::CpduReseatNack()
{
  memset(sz_username, 0, sizeof(sz_username));
}

void CpduReseatNack::grok(SOCKET sd, CTable* table)
{
  // Copy username zero-terminated string
  pdu_reseatack* data = (pdu_reseatack*)(value_ + PDU_HEADERSIZE);
  memcpy(sz_username, data->username, PDU_STRINGSIZE);

  CPlayer* player = CLounge::Inst()->findPlayer(sz_username);
  if (player)
  {
    CBaseTournamentQueue* tq =
      CBaseTournamentQueue::FindQueue(table);

    if (tq)
    {
      CpduQueueFull pdu;
      pdu.sendQueueFull(tq->getQueueNumber(),
                        player,
                        QF_AlreadyPlaying);
    }
    else
    {
      string s;
      s = string("CpduReseatNack: no tournament found (") + string(sz_username) + string(")");
      Sys_LogError(s.c_str());
    }
  }

  // this pdu is done
  delete this;
}
