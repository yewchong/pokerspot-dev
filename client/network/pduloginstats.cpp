/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDULoginStats
  Created  : 02/25/2000

  OVERVIEW : Lounge Login Stats PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduloginstats.h"
#ifndef PSPOT_TABLE_MODULE_
  #include "loungedlg/floormap.h"
#endif


BEGIN_NAMESPACE(Network)


void PDULoungeLoginStats::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CFloorMap::Instance()->setStats(players_, tables_);
#endif
}


END_NAMESPACE(Network)