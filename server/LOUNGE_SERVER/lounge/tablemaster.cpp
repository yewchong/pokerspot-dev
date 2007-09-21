/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CTableMaster
  Created  : 04/16/2000

  OVERVIEW : Table master server.

 ****************************************************************************/

#include "lounge/tablemaster.h"
#include "sys.h"

CTableMaster* CTableMaster::Create(u_long addr, u_short port)
{
    SOCKET sd = Sys_SpawnTableMaster(addr, port);
    return new CTableMaster(sd);
}

CTableMaster::CTableMaster(SOCKET sd)
    :
    sd_(sd)
{}

