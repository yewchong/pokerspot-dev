#ifndef __lounge_pdutablelogout_h__
#define __lounge_pdutablelogout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogout
  Created  : 04/30/2000

  OVERVIEW : Table Logout PDU - a table server sends this
             pdu to the Lounge server at exit.

 ****************************************************************************/

#include "pdu.h"


class CpduTableLogout : public Cpdu
{
public:
    CpduTableLogout();
    void grok(SOCKET sd, CTable* table);
};

#endif
