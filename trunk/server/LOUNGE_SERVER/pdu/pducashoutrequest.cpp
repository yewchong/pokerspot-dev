/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCashoutRequest
  Created  : 04/20/2000

  OVERVIEW : Receive-only cash out request PDU - player wants to cash out.

 ****************************************************************************/

#include "pducashoutrequest.h"
#include "pducashoutreply.h"
#include "lounge/player.h"
#include "lounge/lounge.h"

REGISTER_INCOMING_PDU(PDU_Lounge_CashOutRequest, CpduCashoutRequest);

CpduCashoutRequest::CpduCashoutRequest()
{}

void CpduCashoutRequest::grok(SOCKET, CPlayer* player)
{
    u_int16_t amount = getCashoutAmount();


    bool retval = CLounge::Inst()->doCredit(player, amount);

    // If it succeeded, client will poll for results..  Reply when
    // results are in.
      

    // this pdu is done
    delete this;
}
