/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduChat
  Created  : 04/16/2000

  OVERVIEW : Chat pdu.

 ****************************************************************************/

#include "pduchat.h"
#include "pdu/pdunotify.h"
#include "dbhandler/dbinterface.h"

REGISTER_INCOMING_PDU(PDU_Chat, CpduChat);

CpduChat::CpduChat()
{
  type_ = PDU_Chat;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}

void CpduChat::grok(SOCKET sd, CPlayer*)
{
  CdbInterface* db = CdbInterface::Inst();

  if (db && db->checkChatBan(getChatUser()))
  {
    CpduNotify pdu;
    pdu.sendNotifyMessage(sd,
                          "Your chat has been temporarily disabled.",
                          NF_NoCloseConnection);
  }
  else
  {
    Cpdu::broadcastToPlayers();
  }

  delete this;
}


const char* CpduChat::getChatUser() const
{
  // Copy the username to 0-terminated buffer
  static char sz_username[PDU_STRINGSIZE+1];
  memset(sz_username, 0, sizeof(sz_username));

  const char* pChatUser = (value_ + PDU_HEADERSIZE);
  strncpy(sz_username, pChatUser, PDU_STRINGSIZE);

  return sz_username;
};


int CpduChat::broadcastMessage(const char* username, const char* message)
{
  if (PDU_HEADER_SIZE + strlen(username) + strlen(message) >=
      sizeof(value_))
    return -1;

  length_ = PDU_HEADER_SIZE + PDU_STRINGSIZE + strlen(message);
  stuffHeader();

  memcpy(value_ + PDU_HEADER_SIZE, username, strlen(username));
  memcpy(value_ + PDU_HEADER_SIZE + PDU_STRINGSIZE, message, strlen(message));

  return Cpdu::broadcastToPlayers();
}


int CpduChat::sendMessage(SOCKET sd, const char* username, const char* message)
{
  if (PDU_HEADER_SIZE + strlen(username) + strlen(message) >=
      sizeof(value_))
    return -1;

  length_ = PDU_HEADER_SIZE + PDU_STRINGSIZE + strlen(message);
  stuffHeader();

  memcpy(value_ + PDU_HEADER_SIZE, username, strlen(username));
  memcpy(value_ + PDU_HEADER_SIZE + PDU_STRINGSIZE, message, strlen(message));

  return Cpdu::sendTo(sd);
}
