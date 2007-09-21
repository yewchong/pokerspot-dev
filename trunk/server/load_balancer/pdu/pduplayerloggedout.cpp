/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedOut
  Created  : 10/04/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             a player logout/disconnect.

 ****************************************************************************/

#include "pdu/pduplayerloggedout.h"
#include "loadbalancer/loadbalancer.h"


REGISTER_INCOMING_PDU(PDU_LB_PlayerLoggedOut, CpduPlayerLoggedOut);


CpduPlayerLoggedOut::CpduPlayerLoggedOut()
{}
  

void CpduPlayerLoggedOut::grok(SOCKET sd, CLounge* lounge)
{
  // Copy the username to zero-terminated string
  char sz_username[PDU_STRINGSIZE + 1];
  char* buf = value_ + PDU_HEADERSIZE;
  strncpy(sz_username, buf, PDU_STRINGSIZE);

  if (sz_username[0] != '\0' && lounge != NULL)
    CLB::Inst()->unregisterLoungePlayer(lounge, sz_username);

  // this pdu is done
  delete this;
}
