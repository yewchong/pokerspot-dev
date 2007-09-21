/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoginReject
  Created  : 04/18/2000

  OVERVIEW : Login reject pdu. Table server does not allow login retries
             because the user has been authenticated already by the lounge
             server.

 ****************************************************************************/

#include "pduloginreject.h"
#include "../player/player.h"

CpduLoginReject::CpduLoginReject()
{
    type_ = PDU_Setup_TableReject;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

int CpduLoginReject::sendReject(SOCKET sd)
{
    return sendTo(sd);
}