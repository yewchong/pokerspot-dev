/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSitdown
  Created  : 04/18/2000

  OVERVIEW : Lounge sitdown PDU.

 ****************************************************************************/

#include "pdusitdown.h"
#include "lounge/lounge.h"
#include "lounge/waitinglist.h"
#include "loadbalancer/pdurouteloungepdu.h"


REGISTER_INCOMING_PDU(PDU_Lounge_SitDown, CpduSitdown);

CpduSitdown::CpduSitdown()
{}

void CpduSitdown::grok(SOCKET sd, CPlayer* player)
{
  CLounge* lounge = CLounge::Inst();
  int queueNumber = getQueueNumber();

  CWaitingList* wlist = lounge->findWaitingList(queueNumber);
  if (wlist && wlist->isOwner())
  {
    // This lounge is the owner of the waiting list -
    // implement the seating logic
    lounge->queuePlayer(player, queueNumber);
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
