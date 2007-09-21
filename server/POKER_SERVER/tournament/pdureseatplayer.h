#ifndef __table_pdureseatplayer_h__
#define __table_pdureseatplayer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduReseatPlayer
  Created  : 07/07/2000

  OVERVIEW : Send-only tournament reseat player pdu.
             Lounge server tells the (old) table server to seat
             a player at a new table. The old table server forwards
             this to the client which closes its connection and
             connects to the new table server.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

class CpduReseatPlayer : public Cpdu
{
public:
    CpduReseatPlayer();
    void grok(SOCKET sd, CTable*);
};


#endif
