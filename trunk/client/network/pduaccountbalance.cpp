/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUAccountBalance
  Created  : 02/20/2000

  OVERVIEW : Account balance PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduaccountbalance.h"
#include "loungedlg/cashierdlg.h"


BEGIN_NAMESPACE(Network)


void PDUAccountBalance::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CCashierDlg::SetBalance(balance_);
#endif
}


END_NAMESPACE(Network)

