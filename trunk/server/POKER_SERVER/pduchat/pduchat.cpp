#include "pduchat.h"
#include "table/table.h"
#include "pdunotify/pdunotify.h"

REGISTER_INCOMING_PDU(PDU_Chat, CpduChat);

CpduChat::CpduChat()
{}

CpduChat::CpduChat(Cpdu *pCpdu)
{
	type_ = pCpdu->type_;
	memcpy(value_, pCpdu->value_, MAXBUFFERSIZE);
	length_ = pCpdu->length_;
};

void CpduChat::grok(SOCKET sd, CTable* table)
{
    char formattedText[MAXCHATSIZE];
    memset(formattedText, 0, MAXCHATSIZE);

    CdbInterface *db = table->getDbase();
    if (db && db->checkChatBan(getChatUser()))
    {
      CpduNotify pdu;
      pdu.sendNotifyMessage(sd,
        "Your chat has been temporarily disabled.",
        NF_NoCloseConnection);
    }
    else
    {
      // Insert code here to determine if this is an observer.. prepend
      // "(observer)" if so..
      // If the player is not one of the players sitting at
      // the table, then he is an observer
      CPlayer* p = table->getPlayerRawSocket(sd);
      if (p == NULL)
      {   // This is an observer chat message
  
          const char* s = "(observer) ";
          int l = strlen(s);

          if (l + length_ < MAXBUFFERSIZE)
          {
              char buf[MAXBUFFERSIZE];
              // Copy user name to buf
              memcpy(buf, value_ + PDU_HEADERSIZE, PDUSTRINGSIZE);
              // Append (observer)
              memcpy(buf + PDUSTRINGSIZE, s, l);
              // Copy message
              memcpy(buf + PDUSTRINGSIZE + l,
                     value_ + PDU_HEADERSIZE + PDUSTRINGSIZE,
                     length_ - PDU_HEADERSIZE - PDUSTRINGSIZE);
              // Copy temporary buffer back to send buffer            
              memcpy(value_ + PDU_HEADERSIZE, buf, length_ - PDU_HEADERSIZE + l);
              length_ += l;
              stuffHeader();
          }
      }

      // Echo the chat to all players
      Cpdu::broadcastTo(table->getRing());

      sprintf(formattedText, "[Player Chat] %s: %s", getChatUser(),
              getChatMsg());

      // Store chat message into our database
      table->setLog(formattedText);
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

