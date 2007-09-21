#ifndef __lounge_tablemaster_h__
#define __lounge_tablemaster_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTableMaster
  Created  : 04/16/2000

  OVERVIEW : Table master server.

 ****************************************************************************/

#include "compat.h"
#include "includes.h"

class CTableMaster
{
public:
    static CTableMaster* Create(u_long addr, u_short port);

private:
    CTableMaster(SOCKET);

    SOCKET sd_;
};


#endif