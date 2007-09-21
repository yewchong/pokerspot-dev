/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeLogin
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             start-up.

 ****************************************************************************/

#include "pdu/pduloungelogin.h"
#include "pdu/pduloungeinfo.h"
#include "pdu/pduwaitinglistconfig.h"
#include "loadbalancer/lounge.h"


REGISTER_INCOMING_PDU(PDU_LB_LoungeLogin, CpduLoungeLogin);


CpduLoungeLogin::CpduLoungeLogin()
{}


void CpduLoungeLogin::grok(SOCKET, CLounge* lounge)
{
  // XXX TODO: authentication. SSL or own?

  CpduLoungeInfo pdu;
  pdu.sendNewLoungeInfo(lounge);

  // Finish the login by setting the port
  lounge->setPort(getPort());

  // When new lounge logs in, send waiting list
  // owner configuration to all
  CpduWaitinglistConfig pdu2;
  pdu2.sendWaitinglistOwners();

  // this pdu is done
  delete this;
}

