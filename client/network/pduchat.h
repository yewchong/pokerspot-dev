#ifndef __network_pduchat_h__
#define __network_pduchat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUChat
  Created  : 10.11.1999

  OVERVIEW : Chat PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUChat : public PDUHeader
{
  string_t  username_;
  message_t message_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(string_t); }


  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Chat PDU ---");
    if (message_ != 0)
    {
      CString s, n = MakeString(username_);
      s.Format("   chat: %s: %s", (LPCSTR)n, message_);
      PDU_DEBUG((LPCSTR)s); 
    }
#endif
    u_byte_t* pbyte = (u_byte_t*)PDUHeader::write(buffer);

    // Write username
    memcpy(pbyte, username_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;

    // Write message
    size_t msglen = lstrlen(message_);
    size_t len = length_ - sizeOf();
    assert(msglen == (len - 1));
    memcpy(pbyte, message_, msglen);
    pbyte += msglen;
    *pbyte++ = '\0';

    return pbyte;
  }

  PDUChat(const char* username,
          const char* password,
          const char* msg,
          int         msglen)
    :
    PDUHeader(PDU_Chat,
              sizeOf() + msglen + 1,
              username, password),
    message_(0)
  {
    if (username != NULL && *username != '\0')
      strncpy(username_, username, PDU_STRINGSIZE);
    else
      memset(username_, 0, sizeof(username_));

    message_ = new char[msglen + 1];
    if (message_)
    {
      memcpy(message_, msg, msglen);
      message_[msglen] = '\0';
    }
  }

  PDUChat(const char* buf)
    :
    PDUHeader(buf),
    message_ (0)
  {
    strncpy(username_, &buf[PDU_HEADER_SIZE], PDU_STRINGSIZE);

    // Point after username
    int msgstart = PDU_HEADER_SIZE + PDU_STRINGSIZE;

    int msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen+1];
      if (message_)
      {
        memcpy(message_, &buf[msgstart], msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDUChat()
  {
    delete [] message_;
    message_ = 0;
  }  

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Chat PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    if (message_ != 0)
    {
      CString s;
      s.Format("   message: %s", message_);
      PDU_DEBUG((LPCSTR)s); 
    }
#endif
  }
};

END_NAMESPACE(Network)


#endif