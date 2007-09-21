/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedIn
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             successfull player login

 ****************************************************************************/


#include "loadbalancer/pduplayerloggedin.h"

CpduPlayerLoggedIn::CpduPlayerLoggedIn()
{
  type_ = PDU_LB_PlayerLoggedIn;
  length_ = PDU_HEADERSIZE + sizeof(pdu_playerloggedin);
  stuffHeader();
}

int CpduPlayerLoggedIn::sendPlayerLoggedIn(SOCKET sd,
                                           const char* username)
{
  pdu_playerloggedin pdu;
  memset(&pdu, 0, sizeof(pdu));
  
  strncpy(pdu.username, username, PDU_STRINGSIZE);  
  memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));
  
  return sendTo(sd);
}
