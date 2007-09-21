/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedOut
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             player logout or disconnect.

 ****************************************************************************/

#include "loadbalancer/pduplayerloggedout.h"
#include "loadbalancer/pduplayerloggedin.h"

CpduPlayerLoggedOut::CpduPlayerLoggedOut()
{
  type_ = PDU_LB_PlayerLoggedOut;
  length_ = PDU_HEADERSIZE + sizeof(pdu_playerloggedin);
  stuffHeader();
}

int CpduPlayerLoggedOut::sendPlayerLoggedOut(SOCKET sd,
                                             const char* username)
{
  pdu_playerloggedin pdu;
  memset(&pdu, 0, sizeof(pdu));
  
  strncpy(pdu.username, username, PDU_STRINGSIZE);  
  memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));
  
  return sendTo(sd);
}
