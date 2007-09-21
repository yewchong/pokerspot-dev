#ifndef __table_pduplayerunseated_h__
#define __table_pduplayerunseated_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerSeated
  Created  : 07/10/2000

  OVERVIEW : Send-only player unseated pdu.
             Table server tells lounge server that a player was logged out.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"


class CpduPlayerUnseated : public Cpdu
{
public:
    CpduPlayerUnseated();
    int sendPlayerUnseated(SOCKET      sd,
                           const char* username);
};


#endif