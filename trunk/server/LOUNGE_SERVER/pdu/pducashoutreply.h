#ifndef __lounge_pducashoutreply_h__
#define __lounge_pducashoutreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCashoutReply
  Created  : 04/20/2000

  OVERVIEW : Send-only cash out reply PDU.

 ****************************************************************************/

#include "pdu.h"

enum CashoutIndicator
{
    CI_Denied   = 0,
    CI_Accepted = 1
};

class CpduCashoutReply : public Cpdu
{
public:
    CpduCashoutReply();
    int sendCashoutReply(CPlayer* player, u_int16_t indicator);
};

#endif
