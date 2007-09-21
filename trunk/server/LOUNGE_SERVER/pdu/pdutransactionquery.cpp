/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTransactionQuery
  Created  : 05/26/2000

  OVERVIEW : Receive-only Transaction Query PDU.

 ****************************************************************************/

#include "pdutransactionquery.h"
#include "pdutransactionresponse.h"
#include "lounge/player.h"
#include "lounge/lounge.h"


REGISTER_INCOMING_PDU(PDU_Lounge_TransactionQuery, CpduTransactionQuery);

CpduTransactionQuery::CpduTransactionQuery()
{}

void CpduTransactionQuery::grok(SOCKET sd, CPlayer* player)
{
  const char* retmsg = CLounge::Inst()->checkTransaction(player);

  if (retmsg != NULL)
  {
    CpduTransactionResponse pdu;
    pdu.sendResponse(sd, retmsg);
  }
    
  // this pdu is done
  delete this;
}
