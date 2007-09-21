#ifndef __network_pduerror_h__
#define __network_pduerror_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : pduerror.h
  Created  : 12.12.1999

  OVERVIEW : Error PDUs.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUErrorClient : public PDUHeader
{ // Originated from server
  message_t message_;

  void execute(GameState*);
  size_t messageLength() const { return length_ - sizeOf(); }

  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Client Error PDU ---");
#endif
    u_byte_t* pbyte = (u_byte_t*)PDUHeader::write(buffer);
    size_t msglen = lstrlen(message_);
    size_t len = length_ - sizeOf();
    assert(msglen == (len - 1));
    memcpy(pbyte, message_, msglen);
    pbyte += msglen;
    *pbyte++ = '\0';
    return pbyte;
  }

  PDUErrorClient(const char* username,
                 const char* password,
                 const char* msg, int msglen)
    :
    PDUHeader(PDU_Error_Client,
              sizeOf() + msglen + 1,
              username, password),
    message_ (0)
  {
    assert(msglen < PDU_MESSAGE_LEN);
    message_ = new char[msglen + 1];    
    if (message_)
    {
      memcpy(message_, msg, msglen);
      message_[msglen] = '\0';
    }
  }

  PDUErrorClient(const char* buf)
    :
    PDUHeader(buf),
    message_(0)
  {
    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, &buf[PDU_HEADER_SIZE], msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDUErrorClient()
  {
    delete [] message_;
    message_ = 0;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Client Error PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
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


struct PDUErrorServer : public PDUHeader
{
  // Originated from client
  message_t message_;

  size_t messageLength() const { return length_ - sizeOf(); }

  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Server Error PDU ---");
#endif
    u_byte_t* pbyte = (u_byte_t*)PDUHeader::write(buffer);
    size_t msglen = lstrlen(message_);
    size_t len = length_ - sizeOf();
    assert(msglen == (len - 1));
    memcpy(pbyte, message_, msglen);
    pbyte += msglen;
    *pbyte++ = '\0';
    return pbyte;
  }

  PDUErrorServer(const char* username,
                 const char* password,
                 const char* msg, int msglen)
    :
    PDUHeader(PDU_Error_Server,
              sizeOf() + msglen + 1,
              username, password),
    message_ (0)
  {
    assert(msglen < PDU_MESSAGE_LEN);
    message_ = new char[msglen + 1];    
    if (message_)
    {
      memcpy(message_, msg, msglen);
      message_[msglen] = '\0';
    }
  }

  PDUErrorServer(const char* buf)
    :
    PDUHeader(buf),
    message_(0)
  {
    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, &buf[PDU_HEADER_SIZE], msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDUErrorServer()
  {
    delete [] message_;
    message_ = 0;
  }

};

END_NAMESPACE(Network)


#endif