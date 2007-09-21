#ifndef __lounge_pdubuyinreply_h__
#define __lounge_pdubuyinreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBuyinReply
  Created  : 04/20/2000

  OVERVIEW : Send-only buy in reply PDU.

 ****************************************************************************/

#include "pdu.h"

enum BuyinIndicator
{
    BI_Denied   = 0,
    BI_Accepted = 1,
};

class CpduBuyinReply : public Cpdu
{
public:
    CpduBuyinReply();
    int sendBuyinReply(CPlayer* player, u_int16_t indicator);
};

#endif
