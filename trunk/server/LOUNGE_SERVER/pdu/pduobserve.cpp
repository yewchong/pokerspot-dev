/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduObserve
  Created  : 05/26/2000

  OVERVIEW : Lounge observe PDU - player wants to observe a table.

 ****************************************************************************/

#include "pduobserve.h"
#include "pduplayerobservequery.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Lounge_Observe, CpduObserve);

CpduObserve::CpduObserve()
{}

void CpduObserve::grok(SOCKET sd, CPlayer* player)
{
  CTable* table = CLounge::Inst()->findTable(getTableNumber());

  if (table)
  {
    CpduPlayerObserveQuery pdu;
    pdu.sendObserveQuery(table, player);
  }

  // this pdu is done
  delete this;
}
