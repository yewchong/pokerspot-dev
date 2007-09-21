/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogout
  Created  : 04/30/2000

  OVERVIEW : Send-only Table Logout PDU - a table server sends this
             pdu to the Lounge server at exit.

 ****************************************************************************/

#include "pdutablelogout.h"


CpduTableLogout::CpduTableLogout()
{
    type_ = PDU_Server_TableLogout;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

int CpduTableLogout::sendTableLogout(SOCKET sd)
{
    return sendTo(sd);
}

