/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBuyinRequest
  Created  : 04/20/2000

  OVERVIEW : Receive-only buy in request PDU - player wants to buy in.

 ****************************************************************************/

#include "pdubuyinrequest.h"
#include "pdubuyinreply.h"
#include "lounge/player.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Lounge_BuyInRequest, CpduBuyinRequest);

CpduBuyinRequest::CpduBuyinRequest()
{}

void CpduBuyinRequest::grok(SOCKET, CPlayer* player)
{
    u_int16_t card = getCardIndex();
    u_int16_t amount = getBuyinAmount();

    
    // Needs to stuff buyin request into transactions table now...
    bool retval = CLounge::Inst()->doDebit(player, amount, card);


    if (!retval)
    {   // If adding the transaction to the table didn't work..
        CpduBuyinReply pdu;
        pdu.sendBuyinReply(player, BI_Denied);  // Deny this transaction
    }

    // this pdu is done
    delete this;
}
