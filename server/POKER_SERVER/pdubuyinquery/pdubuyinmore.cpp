/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBuyInMore
  Created  : 05/11/2000

  OVERVIEW : Receive-only Buy-In More PDU - player wants to bring 
             more chips to the table.

 ****************************************************************************/

#include "pdubuyinmore.h"
#include "pdubuyinquery.h"
#include "pdunotify/pdunotify.h"
#include "table/table.h"

REGISTER_INCOMING_PDU(PDU_Setup_BuyinMore, CpduBuyInMore);

CpduBuyInMore::CpduBuyInMore()
{}

void CpduBuyInMore::grok(SOCKET sd, CTable* table)
{
  CPlayer* player = table->getPlayerRawSocket(sd);
  
  if (player)
  {
    CChips chips;
    table->getDbase()->getChips(player, chips);
    if (chips > 0)
    {
      // The real work is done by the buy-in
      // query pdu
      CpduBuyinQuery pdu(table);
      pdu.sendBuyinQuery(player);
    }
    else
    {
      CpduNotify n;
      n.sendNotifyMessage(player->getConnfd(),
                          "No more chips to buy in.",
                          NF_NoCloseConnection);
    }
  }
  
  // this pdu is done
  delete this;
}
