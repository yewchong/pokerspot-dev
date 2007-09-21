#ifndef __lounge_pdustatus_h__
#define __lounge_pdustatus_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduStatus
  Created  : 04/18/2000

  OVERVIEW : Lounge status PDU.

 ****************************************************************************/


#include "pdu.h"

class CpduStatus : public Cpdu
{
public:
    CpduStatus();
    int sendGameNames(SOCKET sd);
};


#endif