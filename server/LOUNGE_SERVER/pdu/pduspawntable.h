#ifndef __lounge_pduspawntable_h__
#define __lounge_pduspawntable_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSpawnTable
  Created  : 04/20/2000

  OVERVIEW : Send-only spawn table PDU - Lounge server
             wants to spawn a table server.

 ****************************************************************************/

#include "pdu.h"

struct pdu_spawntable
{
    u_int16_t gametype;
    u_int16_t tablenumber;
    u_int16_t tablehigh;
    u_int16_t tablelow;
};

class CpduSpawnTable : public Cpdu
{
public:
    CpduSpawnTable();
    int sendSpawnTable(SOCKET sd,
                       u_int16_t gameType,
                       u_int16_t tableNumber,
                       u_int16_t tableLow,
                       u_int16_t tableHigh);
};

#endif
