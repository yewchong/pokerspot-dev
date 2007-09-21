
/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveReply
  Created  : 05/27/2000

  OVERVIEW : Send-only Player Observe Reply PDU - table server replies
             whether it is ok for a player to observe the table.

 ****************************************************************************/


#include "pduplayerobservereply.h"

struct pdu_playerobservereply
{
  u_int16_t tablenumber;
  char username[PDU_STRINGSIZE];
  u_int16_t reply;
};


CpduPlayerObserveReply::CpduPlayerObserveReply()
{
  type_ = PDU_Server_PlayerObserveReply;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_playerobservereply);
  stuffHeader();
}

int CpduPlayerObserveReply::sendObserveReply(SOCKET sd,
                                             u_int16_t tableNumber,
                                             const char* username,
                                             ObserveReply reply)
{
  pdu_playerobservereply pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.tablenumber = htons(tableNumber);
  strncpy(pdu.username, username, PDU_STRINGSIZE);
  pdu.reply = htons((u_int16_t)reply);
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return sendTo(sd);
}