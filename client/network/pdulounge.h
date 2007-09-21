#ifndef __network_pdulounge_h__
#define __network_pdulounge_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : pdulounge.h
  Created  : 10.11.1999

  OVERVIEW : Lounge PDUs.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDULoungeLogin : public PDUHeader
{
  string_t username_;
  string_t password_;
  u_int16_t platform_;
  u_byte_t checksum_[CLIENT_CHECKSUM_SIZE];

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t) + 
                                 2 * sizeof(string_t) +
                                 CLIENT_CHECKSUM_SIZE; }

  // Originated from client
  virtual u_byte_t* write(char* buffer) const
  {
    PDU_DEBUG("--- Send: Lounge Login PDU ---");

    u_byte_t* pbyte = PDUHeader::write(buffer);

    // Write username & password
    memcpy(pbyte, username_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;
    memcpy(pbyte, password_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;
    u_int16_t* pint = (u_int16_t*)pbyte;
    *pint++ = htons(platform_);
    pbyte = (u_byte_t*)pint;
    memcpy(pbyte, checksum_, CLIENT_CHECKSUM_SIZE);
    pbyte += CLIENT_CHECKSUM_SIZE;

    return pbyte;
  }


  PDULoungeLogin(const char* username,
                 const char* password,
                 u_int16_t   platform,
                 u_byte_t*   checksum)
    :
    PDUHeader(PDU_Lounge_Login, sizeOf(),
              username, password),
    platform_(platform)
  {
    if (username != NULL && *username != '\0')
      strncpy(username_, username, PDU_STRINGSIZE);
    else
      memset(username_, 0, sizeof(username_));

    if (password != NULL && *password != '\0')
      strncpy(password_, password, PDU_STRINGSIZE);
    else
      memset(password_, 0, PDU_STRINGSIZE);

    memcpy(checksum_, checksum, CLIENT_CHECKSUM_SIZE);
  }
};


struct PDULoungeAccept : public PDUHeader
{
  // Originated from server
  string_t username_;
  string_t password_;
  message_t message_;

  // Constant size does not take message into account
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(string_t); }

  void execute(GameState*);
  size_t messageLength() const { return length_ - sizeOf(); }

  virtual u_byte_t* write(char* buffer) const
  {
    u_byte_t* pbyte = (u_byte_t*)PDUHeader::write(buffer);

    // Write username & password
    memcpy(pbyte, username_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;
    memcpy(pbyte, password_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;

    // Write message
    size_t msglen   = lstrlen(message_);
    size_t len      = length_ - sizeOf();
    assert(msglen == (len - 1));
    memcpy(pbyte, message_, msglen);
    pbyte           += msglen;
    *pbyte++        = '\0';

    return pbyte;
  }

  PDULoungeAccept(const char* username,
                  const char* password,
                  const char* msg, int msglen)
    :
    PDUHeader(PDU_Lounge_Accept,
              sizeOf() + msglen + 1,
              username, password),
    message_ (0)
  {
    if (username != NULL && *username != '\0')
      strncpy(username_, username, PDU_STRINGSIZE);
    else
      memset(username_, 0, sizeof(username_));

    if (password != NULL && *password != '\0')
      strncpy(password_, password, PDU_STRINGSIZE);
    else
      memset(password_, 0, PDU_STRINGSIZE);

    message_ = new char[msglen + 1];    
    if (message_)
    {
      memcpy(message_, msg, msglen);
      message_[msglen] = '\0';
    }
  }

  PDULoungeAccept(const char* buf)
    :
    PDUHeader(buf),
    message_(0)
  {
    const char* pbyte = &buf[PDU_HEADER_SIZE];
    strncpy(username_, pbyte, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;
    strncpy(password_, pbyte, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;

    size_t msglen = length_ - sizeOf();
    if (msglen > 0)
    {
      message_ = new char[msglen + 1];
      if (message_)
      {
        memcpy(message_, pbyte, msglen);
        message_[msglen] = '\0';
      }
    }
  }

  ~PDULoungeAccept()
  {
    delete [] message_;
    message_ = 0;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Accept PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    if (message_ != 0)
    {
      char buf[10];
      strncpy(buf, message_, 9);
      buf[9] = '\0';
      CString s;
      s.Format("   user: %s start of message: %s",
               (LPCSTR)MakeString(username_), message_);
      PDU_DEBUG((LPCSTR)s);
    }
#endif
  }
};


struct PDULoungeReject : public PDUHeader
{
  // Originated from server
  u_int16_t flag_;

  enum RejectFlag
  {
    RF_Allow_Retry = 1,
    RF_No_Retry    = 2
  };

  void execute(GameState*);

  PDULoungeReject(const char* buf)
    :
    PDUHeader(buf),
    flag_    (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    flag_ = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Reject PDU ---");
    PDUHeader::checkValid();
  }
};


struct PDULoungeSurvey : public PDUHeader
{
  // Originated from client

  virtual u_byte_t* write(char* buffer) const
  {
    PDU_DEBUG("--- Send: Lounge Survey PDU ---");
    u_byte_t* p = PDUHeader::write(buffer);
    return p;
  }

  PDULoungeSurvey(const char* username,
                  const char* password)
    :
    PDUHeader(PDU_Lounge_Survey, sizeOf(),
              username, password)
  {}

  PDULoungeSurvey(const char* buf)
    :
    PDUHeader(buf)
  {}
};


inline
void SanityCheckNum(u_int16_t& n, int m)
{
  if (n > m)
    n = m;
}

struct PDULoungeStatus : public PDUHeader
{
  // Originated from server
  enum
  {
    MaxTables = 20
  };

  struct QueueEntry
  {
    u_int16_t queuenumber_;
    u_int16_t queuesize_;
    CChips::chip_amount_t lolimit_;
    CChips::chip_amount_t hilimit_;
    u_int16_t queuetype_;
    u_int16_t expansion_;
  };
  QueueEntry* pQueues_;
  u_int16_t   numQueues_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf(); }

  PDULoungeStatus(const char* buf)
    :
    PDUHeader (buf),
    pQueues_  (0),
    numQueues_(0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];

    // Rest of the data is QueueEntries
    size_t extraBytes = length_ - PDU_HEADER_SIZE;
    numQueues_ = extraBytes / sizeof(QueueEntry);
    SanityCheckNum(numQueues_, 100);

    if (numQueues_ > 0)
      pQueues_ = new QueueEntry[numQueues_];

    if (pQueues_)
    {
      memset(pQueues_, 0, numQueues_ * sizeof(QueueEntry));
      QueueEntry* pQueue = pQueues_;
      for (int i = 0; i < numQueues_; i++, pQueue++)
      {
        pQueue->queuenumber_ = ntohs(*pint++);
        pQueue->queuesize_ = ntohs(*pint++);

        CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
        pQueue->lolimit_ = *pchips++; // XXX CCHIPS NOTE: network byte order
        pQueue->hilimit_ = *pchips++;

        pint = (u_int16_t*)pchips;
        pQueue->queuetype_ = ntohs(*pint++);
        pQueue->expansion_ = ntohs(*pint++);
      }
    }
  }

  ~PDULoungeStatus()
  {
    delete [] pQueues_;
    pQueues_ = NULL;
  }
    
  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Status PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("  Queues: %d", numQueues_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


struct PDULoungeSitDown : public PDUHeader
{
  // Originated from client

  u_int16_t queue_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Lounge Sit Down PDU ---");
#endif
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++         = htons(queue_);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   queue: %d ", queue_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }

  PDULoungeSitDown(const char* username,
                   const char* password,
                   u_int16_t   queue)
    :
    PDUHeader(PDU_Lounge_SitDown, sizeOf(),
              username, password),
    queue_(queue)
  {}
};


struct PDULoungeCancelSitDown : public PDUHeader
{
  // Originated from client

  u_int16_t queue_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Lounge Cancel Sit Down PDU ---");
#endif
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++         = htons(queue_);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   queue: %d ", queue_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }

  PDULoungeCancelSitDown(const char* username,
                         const char* password,
                         u_int16_t   queue)
    :
    PDUHeader(PDU_Lounge_CancelSitDown,
              sizeOf(), username, password),
    queue_(queue)
  {}
};


struct PDULoungeObserve : public PDUHeader
{
  // Originated from client

  u_int16_t table_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Lounge Observe PDU ---");
#endif
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++         = htons(table_);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   table: %d ", table_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }

  PDULoungeObserve(u_int16_t table)
    :
    PDUHeader(PDU_Lounge_Observe,
              sizeOf(), NULL, NULL),
    table_(table)
  {}
};


struct PDULoungeTableFull : public PDUHeader
{
  // Originated from server

  u_int16_t table_;
  u_int16_t queueSize_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++         = htons(table_);
    *pint++         = htons(queueSize_);
    return (u_byte_t*)pint;
  }

  PDULoungeTableFull()
    :
    PDUHeader(PDU_Lounge_TableFull, sizeOf(),
              NULL, NULL)
  {}

  PDULoungeTableFull(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    table_          = ntohs(*pint++);
    queueSize_      = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Table Full PDU ---");
    PDUHeader::checkValid();
  }
};


struct PDULoungeSitting : public PDUHeader
{
  // Originated from server

  u_int16_t table_;
  u_int16_t port_;
  u_int32_t host_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t) +
                                 sizeof(u_int32_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint  = (u_int16_t*)PDUHeader::write(buf);
    *pint++          = htons(table_);
    *pint++          = htons(port_);
    u_int32_t* plong = (u_int32_t*)pint;
    *plong++         = htonl(host_);
    return (u_byte_t*)plong;
  }

  PDULoungeSitting(const char* username,
                   const char* password,
                   u_int16_t table,
                   u_int32_t host,
                   u_int16_t port)
    :
    PDUHeader(PDU_Lounge_Sitting, sizeOf(),
              username, password),
    table_   (table),
    port_    (port),
    host_    (host)
  {}

  PDULoungeSitting(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint  = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    table_           = ntohs(*pint++);
    port_            = ntohs(*pint++);
    u_int32_t* plong = (u_int32_t*)pint;
    host_            = ntohl(*plong++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Sitting PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   table: %d host: %ld port %d", table_, host_, port_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


struct PDULoungeObserving : public PDUHeader
{
  // Originated from server - identical to the sitting pdu

  u_int16_t table_;
  u_int16_t port_;
  u_int32_t host_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t) +
                                 sizeof(u_int32_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint  = (u_int16_t*)PDUHeader::write(buf);
    *pint++          = htons(table_);
    *pint++          = htons(port_);
    u_int32_t* plong = (u_int32_t*)pint;
    *plong++         = htonl(host_);
    return (u_byte_t*)plong;
  }


  PDULoungeObserving(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint  = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    table_           = ntohs(*pint++);
    port_            = ntohs(*pint++);
    u_int32_t* plong = (u_int32_t*)pint;
    host_            = ntohl(*plong++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Observing PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   table: %d host: %ld port %d", table_, host_, port_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


struct PDULoungeLogout : public PDUHeader
{
  // Originated from client
  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Lounge Logout PDU ---");
#endif
    u_byte_t* p = PDUHeader::write(buffer);
    return p;
  }

  PDULoungeLogout(const char* username,
                  const char* password)
    :
    PDUHeader(PDU_Lounge_Logout, sizeOf(),
              username, password)
  {}

  PDULoungeLogout(const char* buf)
    :
    PDUHeader(buf)
  {}
};


struct PDULoungeLogoutCertain : public PDUHeader
{
  // Originated from client
  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Lounge Logout Certain PDU ---");
#endif
    u_byte_t* p = PDUHeader::write(buffer);
    return p;
  }

  PDULoungeLogoutCertain(const char* username,
                         const char* password)
    :
    PDUHeader(PDU_Lounge_LogoutCertain, sizeOf(),
              username, password)
  {}

  PDULoungeLogoutCertain(const char* buf)
    :
    PDUHeader(buf)
  {}

};


struct PDULoungeLogoutWarning : public PDUHeader
{
  void execute(GameState*);

  PDULoungeLogoutWarning(const char* username,
                         const char* password)
    :
    PDUHeader(PDU_Lounge_LogoutWarning, sizeOf(),
              username, password)
  {}

  PDULoungeLogoutWarning(const char* buf)
    :
    PDUHeader(buf)
  {}

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Logout Warning PDU ---");
    PDUHeader::checkValid();
  }
};


struct PDULoungeLogoutConfirmed : public PDUHeader
{
  // Originated from server

  void execute(GameState*);

  PDULoungeLogoutConfirmed(const char* username,
                           const char* password)
    :
    PDUHeader(PDU_Lounge_LogoutConfirmed, sizeOf(),
              username, password)
  {}

  PDULoungeLogoutConfirmed(const char* buf)
    :
    PDUHeader(buf)
  {}

  void checkValid()
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Recv: Lounge Logout Confirmed PDU ---");
#endif
    PDUHeader::checkValid();
  }
};


struct PDULoungeTableLogout : public PDUHeader
{
  u_int16_t table_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Lounge Table Logout PDU ---");
#endif
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++         = htons(table_);
    return (u_byte_t*)pint;
  }

  PDULoungeTableLogout(const char* username,
                       const char* password,
                       u_int16_t table)
    :
    PDUHeader(PDU_Lounge_TableLogout, sizeOf(),
              username, password),
    table_   (table)
  {}

  PDULoungeTableLogout(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    table_          = ntohs(*pint++);
  }
};


END_NAMESPACE(Network)


#endif