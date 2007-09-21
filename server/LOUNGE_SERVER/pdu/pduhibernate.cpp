/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduHibernate
  Created  : 10/11/2000

  OVERVIEW : The lounge client uses this pdu to tell the lounge server
             whether the connection is active or inactive. This is used
             to conserve resources with the dual-connection lounge client.             

 ****************************************************************************/

#include "pduhibernate.h"
#include "lounge/player.h"

REGISTER_INCOMING_PDU(PDU_Lounge_Hibernate, CpduHibernate);

CpduHibernate::CpduHibernate()
{}

void CpduHibernate::grok(SOCKET sd, CPlayer* player)
{
  // NOT IMPLEMENTED

  u_int16_t flags = getFlags();
  //player->setActive(flags & Active);

  // This pdu is done
  delete this;
}