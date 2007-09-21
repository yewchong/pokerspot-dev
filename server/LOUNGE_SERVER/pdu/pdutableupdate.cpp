/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableUpdate
  Created  : 04/24/2000

  OVERVIEW : Receive-only Table Update PDU - table server sends
             this pdu to inform lounge server about state change.

 ****************************************************************************/

#include "pdutableupdate.h"
#include "lounge/table.h"
#include "lounge/lounge.h"
#include "lounge/waitinglist.h"
    
REGISTER_INCOMING_PDU(PDU_Server_TableUpdate, CpduTableUpdate);

CpduTableUpdate::CpduTableUpdate()
{}

void CpduTableUpdate::grok(SOCKET sd, CTable* table)
{
  u_int16_t oldNumPlayers = table->getNumPlayers();
  table->setNumPlayers(getNumPlayers());

  CWaitingList* wlist =
    CLounge::Inst()->findWaitingListWithTable(table->getNumber());
  if (wlist)
  {
    wlist->tableUpdate(table, oldNumPlayers);
  }

  // This pdu is done
  delete this;
}
