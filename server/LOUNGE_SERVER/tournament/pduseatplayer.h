#ifndef __lounge_pduseatplayer_h__
#define __lounge_pduseatplayer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSeatPlayer
  Created  : 07/07/2000

  OVERVIEW : Send-only tournament seat player pdu.
             Lounge server tells table server to seat a player
             at the table.

 ****************************************************************************/

#include "pdu/pdu.h"

class CChips;

enum SeatFlag
{
    SF_SendAck     = 1,
    SF_CheckZombie = 2
};

class CpduSeatPlayer : public Cpdu
{
public:
    CpduSeatPlayer();
    int sendSeatPlayer(const char*   username,
                       const CChips& chips,
                       u_int16_t     flags,
                       CTable*       table);
};


#endif
