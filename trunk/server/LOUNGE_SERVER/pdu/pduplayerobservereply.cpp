/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveReply
  Created  : 04/25/2000

  OVERVIEW : Receive-only Player Observe Reply PDU - table server replies
             whether it is ok to let a player observe a table.

 ****************************************************************************/

#include "pduplayerobservereply.h"
#include "pdunotify.h"
#include "pduobserving.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Server_PlayerObserveReply, CpduPlayerObserveReply);

CpduPlayerObserveReply::CpduPlayerObserveReply()
{}


void CpduPlayerObserveReply::grok(SOCKET sd, CTable* table)
{
  pdu_playerobservereply* pdu = reinterpret_cast<pdu_playerobservereply*>(value_ + PDU_HEADER_SIZE);
  char sz_username[PDU_STRINGSIZE + 1];
  ::ZeroMemory(sz_username, sizeof(sz_username));
  memcpy(sz_username, pdu->username, PDU_STRINGSIZE);

  // Find the player
  CPlayer* player =
    CLounge::Inst()->findPlayer(sz_username, true);
  
  if (player)
  {
    if (getReply() == OR_Accept)
    {
      CpduObserving pdu;
      pdu.sendObserving(player->getSocket(),
                        table->getNumber(),
                        table->getPort(),
                        table->getAddr());
    }
    else
    {
      char s[200];
      sprintf(s, "Request to observe table %i denied.",
              table->getNumber());
      CpduNotify pdu;
      pdu.sendNotifyMessage(player->getSocket(),
                            s, NF_NoCloseConnection);
    }
  }
  
  // This pdu is done
  delete this;
}
