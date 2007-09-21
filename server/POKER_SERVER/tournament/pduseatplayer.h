#ifndef __table_pduseatplayer_h__
#define __table_pduseatplayer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSeatPlayer
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament seat player pdu.
             Lounge server tells table server to seat a player
             at the table.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

enum SeatFlag
{
    SF_SendAck     = 1,
    SF_CheckZombie = 2
};

class CpduSeatPlayer : public Cpdu
{
public:
    CpduSeatPlayer();
    void grok(SOCKET sd, CTable*);
};


#endif
