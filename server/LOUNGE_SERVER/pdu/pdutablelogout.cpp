/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogout
  Created  : 04/30/2000

  OVERVIEW : Table Logout PDU - a table server sends this
             pdu to the Lounge server at exit.

 ****************************************************************************/

#include "pdutablelogout.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Server_TableLogout, CpduTableLogout);

CpduTableLogout::CpduTableLogout()
{}

void CpduTableLogout::grok(SOCKET sd, CTable* table)
{
  CLounge::Inst()->tableLogout(sd, table);

  // this pdu is done
  delete this;
}

