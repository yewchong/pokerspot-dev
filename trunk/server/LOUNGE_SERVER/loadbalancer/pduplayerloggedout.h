#ifndef __lounge_pduplayerloggedout_h__
#define __lounge_pduplayerloggedout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedOut
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             player logout or disconnect.

 ****************************************************************************/

#include "pdu/pdu.h"

class CpduPlayerLoggedOut : public Cpdu
{
public:
  CpduPlayerLoggedOut();
  int sendPlayerLoggedOut(SOCKET sd,
                          const char* username);
};



#endif