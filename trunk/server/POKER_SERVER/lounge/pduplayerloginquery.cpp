/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginQuery
  Created  : 04/24/2000

  OVERVIEW : Receive-only Player Login Query PDU - lounge server sends
             this pdu to a table server to ask if a player can be
             seated in that table.

 ****************************************************************************/

#include "pduplayerloginquery.h"
#include "pduplayerloginreply.h"
#include "table/table.h"

REGISTER_INCOMING_PDU(PDU_Server_PlayerLoginQuery, CpduPlayerLoginQuery);


CpduPlayerLoginQuery::CpduPlayerLoginQuery()
{}

void CpduPlayerLoginQuery::grok(SOCKET sd, CTable* table)
{
  // Copy username to zero-terminated string
	pdu_playerloginquery* data = (pdu_playerloginquery*)(value_ + PDU_HEADERSIZE);
  memcpy(sz_username, data->username, PDUSTRINGSIZE);
  sz_username[PDUSTRINGSIZE] = '\0';

  u_int32_t ipaddress = getIpAddress();
  const char* name = getUsername();

  LoginReply lr = LR_Accept;

  // The table does necessary checks
  if (table->checkLogin(ipaddress, name))
  {
    lr = LR_Accept;
    table->addLoginEntry(name, ipaddress);
  }
  else
    lr = LR_Reject;

  CpduPlayerLoginReply pdu;
  pdu.sendLoginReply(sd,
                     table->getSeed(),
                     name,
                     lr);

  // This pdu is done
  delete this;
}

