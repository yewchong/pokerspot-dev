/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableQuery
  Created  : 04/24/2000

  OVERVIEW : Receive and send Broad Table Query PDU.

             Player sends this pdu to the Lounge server
             which forwards it to the Table server.

 ****************************************************************************/

#include "pdubroadtablequery.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Lounge_VerboseTableQuery, CpduBroadTableQuery);

CpduBroadTableQuery::CpduBroadTableQuery()
{}

void CpduBroadTableQuery::grok(SOCKET sd, CPlayer* player)
{
  u_int16_t tablenumber = getTableNumber();
  CTable* table = CLounge::Inst()->findTable(tablenumber);
  
  if (table)
  {   // Forward the query to the table
    pdu_broadtablequery_table pdu;
    memset(&pdu, 0, sizeof(pdu));
    
    pdu.lounge = htons(CLounge::Inst()->getLoungeID());
    pdu.playersocket = htons(player->getSocket());
    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
    
    type_ = PDU_Lounge_VerboseTableQuery;
    length_ = PDU_HEADER_SIZE + sizeof(pdu);
    stuffHeader();

 /*
 XXX username must be added to the pdu
        so we know who it will be returned to?
        OR: save lounge number and player socket
        and route the reply back to the same lounge
        who finds the player based on the socket
    XXX sendTo(table);
    table->send(this);
    CTableProxy::sendTo(Cpdu* pdu)
    {
      CpduRouteTablePdu wrapper;
      return wrapper.routePdu(pdu, table);
    }

    LS::CpduRouteTablePdu::grok()
    {
      CTable* table = findTable(getTableNumber());
      return wrappedPdu()->sendTo(table->getSocket());
    }
*/

    sendTo(table);
  }

  // This pdu is done
  delete this;
}