#ifndef __lounge_pduaccountbalance_h__
#define __lounge_pduaccountbalance_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduAccountBalance
  Created  : 04/19/2000

  OVERVIEW : Send-only account balance PDU.

 ****************************************************************************/


#include "pdu.h"

class CChips;

class CpduAccountBalance : public Cpdu
{
public:
  CpduAccountBalance();
  int sendBalance(const CChips& balance,
                  CPlayer* player);
};

#endif