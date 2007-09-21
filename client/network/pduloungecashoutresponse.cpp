/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeCashOutReply
  Created  : 03/13/2000

  OVERVIEW : Lounge Cash Out Reply PDU

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduloungecashoutresponse.h"
#ifndef PSPOT_TABLE_MODULE_
#include "loungedlg/cashierdlg.h"
#endif

BEGIN_NAMESPACE(Network)


void PDULoungeCashOutReply::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CCashierDlg::CashOutResponse(indicator_);
#endif
}


END_NAMESPACE(Network)


