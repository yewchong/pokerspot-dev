#ifndef __lounge_pdulogoutconfirmed_h__
#define __lounge_pdulogoutconfirmed_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLogoutConfirmed
  Created  : 04/18/2000

  OVERVIEW : Logout confirmed PDU

 ****************************************************************************/


#include "pdu.h"

class CpduLogoutConfirmed : public Cpdu
{
public:
    CpduLogoutConfirmed();
};


#endif