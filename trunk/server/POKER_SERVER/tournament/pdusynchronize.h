#ifndef __table_pdusynch_h__
#define __table_pdusynch_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSynchronize
  Created  : 07/14/2000

  OVERVIEW : This PDU is used for synchronizing hands in tournaments.
             Lounge server sends this pdu to tables to ensure hands
             run synchronized. Table server sends this pdu with flag
             SF_HandEnd to notify lounge that a hand has ended.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

enum SyncFlag
{
    SF_Play          = 1,
    SF_Pause         = 2,
    SF_Step          = 3,
    SF_HandEnd       = 4,
    SF_Paused        = 5
};


class CpduSynchronize : public Cpdu
{
public:
    CpduSynchronize();
    int sendSync(SOCKET, SyncFlag);
    void grok(SOCKET, CTable*);
};


#endif
