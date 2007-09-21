/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedIn
  Created  : 10/04/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             a successfull player login.

 ****************************************************************************/


#include "pdu/pduplayerloggedin.h"
#include "loadbalancer/loadbalancer.h"


REGISTER_INCOMING_PDU(PDU_LB_PlayerLoggedIn, CpduPlayerLoggedIn);


CpduPlayerLoggedIn::CpduPlayerLoggedIn()
{}
  

void CpduPlayerLoggedIn::grok(SOCKET sd, CLounge* lounge)
{
  // Copy the username to zero-terminated string
  char sz_username[PDU_STRINGSIZE + 1];
  char* buf = value_ + PDU_HEADERSIZE;
  strncpy(sz_username, buf, PDU_STRINGSIZE);

  if (sz_username[0] != '\0' && lounge != NULL)
    CLB::Inst()->registerLoungePlayer(lounge, sz_username);

  // this pdu is done
  delete this;
}
