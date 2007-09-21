#ifndef __table_pdutablelogout_h__
#define __table_pdutablelogout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogout
  Created  : 04/30/2000

  OVERVIEW : Send-only Table Logout PDU - a table server sends this
             pdu to the Lounge server at exit.

 ****************************************************************************/

#include "../pduhandler/pduhandler.h"


class CpduTableLogout : public Cpdu
{
public:
    CpduTableLogout();
    int sendTableLogout(SOCKET sd);
};

#endif
