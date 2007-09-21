#ifndef __table_pdutablelogin_h__
#define __table_pdutablelogin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogin
  Created  : 04/21/2000

  OVERVIEW : Send-only table login PDU - Table server 
             sends this pdu as a "hello" message to the
             Lounge server.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

class CChips;

class CpduTableLogin : public Cpdu
{
public:
    CpduTableLogin();
    int sendTableLogin(SOCKET sd,
                       u_int16_t gametype,
                       u_int16_t tablenumber,
                       const CChips& tablelow,
                       const CChips& tablehigh,
                       u_int16_t tableport);
};

#endif
