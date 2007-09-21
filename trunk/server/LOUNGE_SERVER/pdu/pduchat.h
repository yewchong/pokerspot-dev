#ifndef __lounge_pduchat_h__
#define __lounge_pduchat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduChat
  Created  : 04/16/2000

  OVERVIEW : Chat pdu.

 ****************************************************************************/


#include "pdu.h"

#define MAXCHATSIZE 256

class CpduChat : public Cpdu
{
public:
  CpduChat();
  const char* getChat() const;
  const char* getChatUser() const;
  const char* getChatMsg() const;
  void grok(SOCKET, CPlayer*);

  int broadcastMessage(const char* username, const char* message);
  int sendMessage(SOCKET sd, const char* username, const char* message);   
};

inline const char* CpduChat::getChat() const
{
	const char* pChatVal = (value_ + PDU_HEADER_SIZE);
	return pChatVal;
};

inline const char* CpduChat::getChatMsg() const
{
    const char* pChatMsg = (value_ + PDU_HEADER_SIZE + PDU_STRINGSIZE);
    return pChatMsg;
};

#endif