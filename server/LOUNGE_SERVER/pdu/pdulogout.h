#ifndef __lounge_pdulogout_h__
#define __lounge_pdulogout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLogout
  Created  : 04/17/2000

  OVERVIEW : Logout PDU.

 ****************************************************************************/

#include "pdu.h"

class CpduLogout : public Cpdu
{
public:
    CpduLogout();
    void grok(SOCKET, CPlayer*);
};


#endif
