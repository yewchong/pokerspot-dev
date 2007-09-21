/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveQuery
  Created  : 05/27/2000

  OVERVIEW : Receive-only Player Observe Query PDU - lounge server sends
             this pdu to a table server to ask if a player can observe
             the table.

 ****************************************************************************/

#include "pduplayerobservequery.h"
#include "pduplayerobservereply.h"
#include "table/table.h"

REGISTER_INCOMING_PDU(PDU_Server_PlayerObserveQuery, CpduPlayerObserveQuery);


CpduPlayerObserveQuery::CpduPlayerObserveQuery()
{}

void CpduPlayerObserveQuery::grok(SOCKET sd, CTable* table)
{
  // Copy username to zero-terminated string
	pdu_playerobservequery* data = (pdu_playerobservequery*)(value_ + PDU_HEADERSIZE);
  memcpy(sz_username_, data->username, PDUSTRINGSIZE);
  sz_username_[PDUSTRINGSIZE] = '\0';

  u_int32_t ipaddress = getIpAddress();
  const char* name = getUsername();

  ObserveReply reply = OR_Accept;

  // The table does necessary checks
  if (table->canObserve(ipaddress, name))
  {
    reply = OR_Accept;
  }
  else
  {
    reply = OR_Reject;
  }

  CpduPlayerObserveReply pdu;
  pdu.sendObserveReply(sd,
                       table->getSeed(),
                       getUsername(),
                       reply);

  // This pdu is done
  delete this;
}

