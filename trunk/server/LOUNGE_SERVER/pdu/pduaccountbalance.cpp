/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduAccountBalance
  Created  : 04/19/2000

  OVERVIEW : Send-only account balance PDU.

 ****************************************************************************/

#include "pduaccountbalance.h"
#include "lounge/player.h"
#include "common/chips.h"

CpduAccountBalance::CpduAccountBalance()
{
  type_ = PDU_Lounge_AccountBalance;
  length_ = PDU_HEADER_SIZE + sizeof(CChips::chip_amount_t);
  stuffHeader();
}

int CpduAccountBalance::sendBalance(const CChips& balance,
                                    CPlayer* player)
{
  char* buf = value_ + PDU_HEADER_SIZE;
  CChips::chip_amount_t balance_n = balance.htonl_rep();
  memcpy(buf, &balance_n, sizeof(balance_n));
  
  return sendTo(player->getSocket());
}

