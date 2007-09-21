/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUPing, PDUPong
  Created  : 02/08/2000

  OVERVIEW : Ping & Pong PDU.

             Upon receiving a Ping PDU, the client immediately
             sends a Pong PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pduping.h"
#include "network/server.h"
#include "ui/global.h"
#include "ui/mainwnd.h"

BEGIN_NAMESPACE(Network)


void PDUPing::execute(GameState*)
{
#ifdef PSPOT_TABLE_MODULE_
  CMainWnd::SetStatusText("The network connection is bad - trying to recover");
#endif

  if (Global::LoungeServer())
    Global::LoungeServer()->sendPongPDU(data_);
  else if (Global::TableServer())
    Global::TableServer()->sendPongPDU(data_);
}


END_NAMESPACE(Network)

