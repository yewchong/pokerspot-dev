#ifndef __pduChat_h__
#define __pduChat_h__

#include "../pduhandler/pduhandler.h"

#define MAXCHATSIZE 256

class CpduChat: public Cpdu
{
public:
    CpduChat();
	CpduChat(Cpdu *);
	const char *getChat() const;
    const char *getChatUser() const;
    const char *getChatMsg() const;
    void grok(SOCKET, CTable*);
    
};

inline const char *CpduChat::getChat() const
{
	const char* pChatVal = (value_ + PDU_HEADERSIZE);
	return pChatVal;
};


inline const char *CpduChat::getChatMsg() const
{
    const char* pChatMsg = (value_ + PDU_HEADERSIZE + 16);
    return pChatMsg;
};

#endif