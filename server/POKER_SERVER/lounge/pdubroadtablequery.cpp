/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableQuery
  Created  : 04/24/2000

  OVERVIEW : Receive and send Broad Table Query PDU.

             Player sends this pdu to the Lounge server
             which forwards it to the Table server.

 ****************************************************************************/

#include "pdubroadtablequery.h"
#include "pdubroadtablereply.h"

REGISTER_INCOMING_PDU(PDU_Lounge_BroadTableQuery, CpduBroadTableQuery);

CpduBroadTableQuery::CpduBroadTableQuery()
{}

void CpduBroadTableQuery::grok(SOCKET sd, CTable* table)
{
    CpduBroadTableReply pdu;
    pdu.sendTableReply(table, sd, getPlayerSocket());

    // This pdu is done
    delete this;
}