/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduBuyinRequest
  Created  : 1/1/2001

  OVERVIEW : This class is used to implement player's buy-in
             when he sits at the table.

 ****************************************************************************/

#include "pdubuyinrequest.h"
#include "sync/sync.h"
#include "table/table.h"
#include "player/player.h"
#include "pduplayer/pduplayer.h"
#include "pdunotify/pdunotify.h"
#include "sys.h"


REGISTER_INCOMING_PDU(PDU_Setup_BuyInRequest, CpduBuyinRequest);


CpduBuyinRequest::CpduBuyinRequest()
{}

CpduBuyinRequest::CpduBuyinRequest(Cpdu* pCpdu)
{
	type_ = pCpdu->type_;
	memcpy(value_, pCpdu->value_, MAXBUFFERSIZE);
	length_ = pCpdu->length_;
}

void CpduBuyinRequest::grok(SOCKET sd, CTable* table)
{
  // Protect against ring rebuild in logic thread
  SingleLock l(&table->getTableMutex());
  l.lock();
  
  CChips chips = getChips();
  CPlayer* player = table->getPlayerRawSocket(sd);
  
  if (player)
  {
    if (chips > 0)
    {
      CChips chipsBoughtIn;
      bool rc = table->getDbase()->buyinChips(player,
                                              chips,
                                              chipsBoughtIn);
      
        if (rc)
      {
        if (chipsBoughtIn == 0)
        {
          CpduNotify n;
          n.sendNotifyMessage(player->getConnfd(),
                              "No more chips to buy in.",
                              NF_NoCloseConnection);
        }
        else
        {
          // XXX: put the chips to chipsBehind!

          // only after the query has succeeded do we buy the player in
          CChips totalChips = player->getChips() + chipsBoughtIn;
          player->setChips(totalChips);

          CpduPlayer pdu(table);
          pdu.sendPlayerRaw(player, PLAYER_CHIPUPDATE);
          if (player->matchState(PLAYER_STATE_LOGGEDIN))
            player->stateChange(PLAYER_STATE_BOUGHTIN);
        }
      }
    }
  }
  else
  {
    char s[200];
    sprintf(s, "CpduBuyinRequest::grok player for socket %d not found", sd);
    Sys_LogError(s);
  }
  
  delete this;
}
