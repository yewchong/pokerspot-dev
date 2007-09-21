/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeTransactionReply
  Created  : 05/16/2000

  OVERVIEW : Lounge Transaction Reply PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduloungetransactionreply.h"
#ifndef PSPOT_TABLE_MODULE_
#include "loungedlg/cashierdlg.h"
#endif

BEGIN_NAMESPACE(Network)

void PDULoungeTransactionReply::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CCashierDlg::TransactionReply(message_);
#endif
}


END_NAMESPACE(Network)


