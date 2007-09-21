#ifndef __lounge_pduobserving_h__
#define __lounge_pduobserving_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduObserving
  Created  : 05/27/2000

  OVERVIEW : Send-only observing PDU; this PDU tells the client that
             the player has been accepted to observe a table.

 ****************************************************************************/

#include "pdu.h"


class CpduObserving : public Cpdu
{
public:
    CpduObserving();
    int sendObserving(SOCKET sd,
                      u_int16_t table,
                      u_int16_t port,
                      u_int32_t host);
};

#endif
