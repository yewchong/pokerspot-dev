#ifndef __network_pduannounce_h__
#define __network_pduannounce_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : struct PDUAnnounce
  Created  : 16.12.1999

  OVERVIEW : Announce PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUAnnounce : public PDUHeader
{
  // Originated from server
  u_int16_t winner_;
  message_t message_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                          sizeof(u_int16_t); }
  size_t messageLength() const { return length_ - sizeOf(); }

  virtual u_byte_t* write(char* buffer) const
  {
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);
    *pint++ = htons(winner_);

    u_byte_t* pbyte = (u_byte_t*)pint;
    size_t msglen   = lstrlen(message_);
    size_t len      = length_ - sizeOf();
    assert(msglen == (len - 1));
    memcpy(pbyte, message_, msglen);
    pbyte           += msglen;
    *pbyte++        = '\0';

    return pbyte;
  }

  PDUAnnounce(const char* username,
              const char* password,
              u_int16_t   winner,
              const char* msg, int msglen)
    :
    PDUHeader(PDU_Play_Announce,
              sizeOf() + msglen + 1,
              username, password),
    winner_  (winner),
    message_ (0)
  {
    message_ = new char[msglen + 1];    
    if (message_)
    {
      memcpy(message_, msg, msglen);
      message_[msglen] = '\0';
    }
  }

  PDUAnnounce(const char* buf)
    :
    PDUHeader(buf),
    winner_  (0),
    message_ (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    winner_ = ntohs(*pint++);

    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, (char*)pint, msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDUAnnounce()
  {
    delete [] message_;
    message_ = 0;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Announce PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    if (message_ != 0)
    {
      char buf[10];
      strncpy(buf, message_, 9);
      buf[9] = '\0';
      CString s;
      s.Format("   start of message: %s", message_);
      PDU_DEBUG((LPCSTR)s);
    }
#endif
  }
};


END_NAMESPACE(Network)

#endif