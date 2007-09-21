/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeLogout
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer when
             it is shutting down.

 ****************************************************************************/


#include "loadbalancer/pduloungelogout.h"


CpduLoungeLogout::CpduLoungeLogout()
{
  type_ = PDU_LB_LoungeLogout;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}


int CpduLoungeLogout::sendLoungeLogout(SOCKET sd)
{ 
  return sendTo(sd);
}
