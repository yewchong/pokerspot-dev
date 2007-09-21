/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableReply
  Created  : 04/24/2000

  OVERVIEW : Receive and send Broad Table Reply PDU.

             Table server sends this pdu as a reply to
             Broad Table Query pdu. Lounge server forwards
             the reply to the player who originated the query-

 ****************************************************************************/

#include "pdubroadtablereply.h"

REGISTER_INCOMING_PDU(PDU_Lounge_VerboseTableReply, CpduBroadTableReply);

CpduBroadTableReply::CpduBroadTableReply()
{
  type_ = PDU_Lounge_VerboseTableReply;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}

void CpduBroadTableReply::grok(SOCKET sd, CTable* table)
{
  // Forward the reply to the player (note:
  // the socket goes as well but it's only 2 bytes!)
  u_int16_t playerSocket = getPlayerSocket();
  sendTo(playerSocket);
  
  // this pdu is done
  delete this;
}
