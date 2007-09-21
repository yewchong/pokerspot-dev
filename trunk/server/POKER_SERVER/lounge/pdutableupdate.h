#ifndef __table_pdutableupdate_h__
#define __table_pdutableupdate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableUpdate
  Created  : 04/24/2000

  OVERVIEW : Send-only Table Update PDU - table server sends
             this pdu to inform lounge server about state change.

 ****************************************************************************/

#include "../pduhandler/pduhandler.h"
    

struct pdu_tableupdate
{
    u_int16_t numplayers;
};

class CpduTableUpdate : public Cpdu
{
public:
    CpduTableUpdate();
    int sendTableUpdate(SOCKET sd, u_int16_t numPlayers);
};

#endif