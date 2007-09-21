/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUAnteAllIn
  Created  : 08/13/2000

  OVERVIEW : This pdu is used to prepare the client when
             a player goes all in in the ante.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduanteallin.h"
#include "ui/tableview.h"
#include "tem/community.h"

BEGIN_NAMESPACE(Network)

void PDUAnteAllIn::execute(GameState* pS)
{
#ifdef PSPOT_TABLE_MODULE_
    CTableView* table = CTableView::Instance();
    Community* pot = static_cast<Community*>(table->getCommunity());
    pot->pot_anteallin(bet_);
#endif
}


END_NAMESPACE(Network)


