/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPlayerInfo
  Created  : 03/01/2000

  OVERVIEW : Player Info PDU. This PDU is sent by the
             server at login to inform the client about
             players sitting at the table.

 ****************************************************************************/

#include "stdafx.h"
#include "network/network.h"
#include "ui/tableview.h"


BEGIN_NAMESPACE(Network)


void PDUPlayerInfo::execute(GameState*)
{
  pView->addPlayer(username, chips_, slot_);
  msg.Format(g_szPlayerJoined, username, (slot_ + 1));
}


END_NAMESPACE(Network)

#endif