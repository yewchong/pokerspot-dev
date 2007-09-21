
/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLogout
  Created  : 04/17/2000

  OVERVIEW : Logout PDU.

 ****************************************************************************/

#include "pdu/pdulogout.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Lounge_Logout, CpduLogout);


CpduLogout::CpduLogout()
{}

void CpduLogout::grok(SOCKET sd, CPlayer* player)
{
  CLounge::Inst()->logout(sd, player);

  // this pdu is done
  delete this;
}
