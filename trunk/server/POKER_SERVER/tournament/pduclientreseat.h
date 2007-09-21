#ifndef __table_pduclientreseat_h__
#define __table_pduclientreseat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduClientReseat
  Created  : 07/26/2000

  OVERVIEW : The table server sends this PDU to the player to
             reseat him at another table.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

struct pdu_reseat
{
  u_int32_t addr;
  u_int16_t port;
};


class CpduClientReseat : public Cpdu
{
public:
    CpduClientReseat();
    int sendReseat(SOCKET sd, u_int32_t addr, u_int16_t port);
};


#endif