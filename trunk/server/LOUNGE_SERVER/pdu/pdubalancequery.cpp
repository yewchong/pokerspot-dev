/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBalanceQuery
  Created  : 04/19/2000

  OVERVIEW : Receive-only balance PDU - player wants to know the
             account balance.

 ****************************************************************************/

#include "pdubalancequery.h"
#include "pduaccountbalance.h"
#include "dbhandler/dbinterface.h"
#include "lounge/player.h"
#include "common/chips.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Lounge_BalanceQuery, CpduBalanceQuery);

CpduBalanceQuery::CpduBalanceQuery()
{}

void CpduBalanceQuery::grok(SOCKET sd, CPlayer* player)
{
  CChips balance;
  
  if (CdbInterface::Inst()->getChips(player, balance))
  {
    CpduAccountBalance pdu;
    pdu.sendBalance(balance, player);
  }
  else
  {
    char s[200];
    sprintf(s, "Can't get player's chips: %s", player->getUsername());
    Sys_LogError(s);
  }
  
  // this pdu is done
  delete this;
}
