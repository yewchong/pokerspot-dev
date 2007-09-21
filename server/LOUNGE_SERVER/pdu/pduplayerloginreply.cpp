/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginReply
  Created  : 04/24/2000

  OVERVIEW : Receive-only Player Login Reply PDU - table server replies
             whether it is ok to seat a player in the table.

 ****************************************************************************/

#include "pduplayerloginreply.h"
#include "lounge/waitinglist.h"
#include "lounge/lounge.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Server_PlayerLoginReply, CpduPlayerLoginReply);

CpduPlayerLoginReply::CpduPlayerLoginReply()
{}

void CpduPlayerLoginReply::grok(SOCKET sd, CTable* table)
{
  pdu_playerloginreply* pdu = reinterpret_cast<pdu_playerloginreply*>(value_ + PDU_HEADER_SIZE);
  char sz_username[PDU_STRINGSIZE + 1];
  ::ZeroMemory(sz_username, sizeof(sz_username));
  memcpy(sz_username, pdu->username, PDU_STRINGSIZE);

  // find the queue
  CWaitingList* wlist = 
    CLounge::Inst()->findWaitingListWithTable(table->getNumber());
  // Find the player
  CPlayer* player =
    CLounge::Inst()->findPlayer(sz_username, true);

  if (wlist)
  {
    if (player)
    {
      if (getReply() == LR_Accept)
        wlist->seatPlayer(player, table);
      else
        wlist->cancelSeat(player, table);
    }
    else
    {
      char buf[100];
      sprintf(buf, "CpduPlayerLoginReply: No player found %s", sz_username);
      Sys_LogError(buf);
    }
  }
  else
  {
    char buf[100];
    sprintf(buf, "CpduPlayerLoginReply: No waiting list for table %d", table->getNumber());
    Sys_LogError(buf);
  }

  // This pdu is done
  delete this;
}
