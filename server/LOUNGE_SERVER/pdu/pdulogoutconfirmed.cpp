/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLogoutConfirmed
  Created  : 04/18/2000

  OVERVIEW : Logout confirmed PDU.

 ****************************************************************************/

#include "pdulogoutconfirmed.h"

CpduLogoutConfirmed::CpduLogoutConfirmed()
{
    type_ = PDU_Lounge_LogoutConfirmed;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}


