/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogin
  Created  : 04/21/2000

  OVERVIEW : Table Login PDU - a table server connects
             to Lounge server using this PDU.

 ****************************************************************************/

#include "pdutablelogin.h"
#include "lounge/table.h"
#include "lounge/lounge.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Server_TableLogin, CpduTableLogin);

CpduTableLogin::CpduTableLogin()
{}

// When a table server logs in, we save the corresponding
// CTable instance to the correct waiting list.
void CpduTableLogin::grok(SOCKET, CTable* table)
{
  table->setData(getGameType(),
                 getTableNumber(),
                 getTableLow(),
                 getTableHigh(),
                 getPort());

  // The table is now properly set up - add it to
  // its waiting list
  CLounge::Inst()->addTableToWaitingList(table, getGameType());

  // this pdu is done
  delete this;
}

