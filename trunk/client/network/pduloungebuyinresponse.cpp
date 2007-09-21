/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeBuyInReply
  Created  : 03/13/2000

  OVERVIEW : Lounge Buy IN Reply PDU

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduloungebuyinresponse.h"
#ifndef PSPOT_TABLE_MODULE_
#include "loungedlg/cashierdlg.h"
#endif

BEGIN_NAMESPACE(Network)


void PDULoungeBuyInReply::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CCashierDlg::BuyInResponse(indicator_);
#endif
}


END_NAMESPACE(Network)
