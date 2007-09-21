/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCancelSitdown
  Created  : 04/18/2000

  OVERVIEW : Lounge cancel sitdown PDU - player wants to be removed
             from a waiting list.

 ****************************************************************************/

#include "pducancelsitdown.h"
#include "lounge/lounge.h"
#include "loadbalancer/pdurouteloungepdu.h"


REGISTER_INCOMING_PDU(PDU_Lounge_CancelSitDown, CpduCancelSitdown);

CpduCancelSitdown::CpduCancelSitdown()
{}

void CpduCancelSitdown::grok(SOCKET sd, CPlayer* player)
{
  CLounge* lounge = CLounge::Inst();
  int queueNumber = getQueueNumber();

  CWaitingList* wlist = lounge->findWaitingList(queueNumber);
  if (wlist && wlist->isOwner())
  {
    // This lounge is the owner of the waiting list -
    // implement the seating logic
    lounge->cancelQueue(player, queueNumber);
  }
  else
  {
    // Another lounge is the owner of the waiting list -
    // route the PDU to the owner Lounge Server.
    CpduRouteLoungePdu wrapper;
    wrapper.routePduByWaitinglist(this,
                                  queueNumber,
                                  player->getUsername());
  }

  // this pdu is done
  delete this;
}
