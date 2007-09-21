
/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeLogin
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             start-up.

 ****************************************************************************/



#include "loadbalancer/pduloungelogin.h"

struct pdu_loungelogin
{
  u_int16_t port;
};

CpduLoungeLogin::CpduLoungeLogin()
{
  type_ = PDU_LB_LoungeLogin;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_loungelogin);
  stuffHeader();
}

int CpduLoungeLogin::sendLoungeLogin(SOCKET sd,
                                     u_int16_t port)
{
  pdu_loungelogin pdu;
  memset(&pdu, 0, sizeof(pdu));
  
  pdu.port = htons(port);
  
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
  
  return sendTo(sd);
}
