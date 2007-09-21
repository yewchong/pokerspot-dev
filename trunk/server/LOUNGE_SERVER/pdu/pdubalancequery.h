#ifndef __lounge_pdubalancequery_h__
#define __lounge_pdubalancequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBalanceQuery
  Created  : 04/19/2000

  OVERVIEW : Receive-only balance PDU - player wants to know the
             account balance.

 ****************************************************************************/

#include "pdu.h"


class CpduBalanceQuery : public Cpdu
{
public:
    CpduBalanceQuery();
    void grok(SOCKET, CPlayer*);
};

#endif
