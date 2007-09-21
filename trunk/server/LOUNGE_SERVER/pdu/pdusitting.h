#ifndef __lounge_pdusitting_h__
#define __lounge_pdusitting_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSitting
  Created  : 04/23/2000

  OVERVIEW : Send-only sitting PDU; this PDU tells the client that
             the player has been taken from the waiting list and
             seated in a table.

 ****************************************************************************/

#include "pdu.h"


class CpduSitting : public Cpdu
{
public:
    CpduSitting();
    int sendSitting(CPlayer*  player,
                    u_int16_t table,
                    u_int16_t port,
                    u_int32_t host);
};

#endif
