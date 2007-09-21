/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginReply
  Created  : 04/24/2000

  OVERVIEW : Send-only Player Login Reply PDU - table server replies
             whether it is ok to seat a player in the table.

 ****************************************************************************/

#include "pduplayerloginreply.h"

struct pdu_playerloginreply
{
  u_int16_t tablenumber;
  char username[PDUSTRINGSIZE];
  u_int16_t reply;
};


CpduPlayerLoginReply::CpduPlayerLoginReply()
{
  type_ = PDU_Server_PlayerLoginReply;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_playerloginreply);
  stuffHeader();
}

int CpduPlayerLoginReply::sendLoginReply(SOCKET sd,
                                         u_int16_t tableNumber,
                                         const char* username,
                                         LoginReply lr)
{
  pdu_playerloginreply pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.tablenumber = htons(tableNumber);
  strncpy(pdu.username, username, PDU_STRINGSIZE);
  pdu.reply = htons((u_int16_t)lr);
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

  return sendTo(sd);
}