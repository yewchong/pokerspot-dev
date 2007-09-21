#ifndef __network_pdusetup_h__
#define __network_pdusetup_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : pdusetup.h
  Created  : 11.12.1999

  OVERVIEW : Setup PDUs.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUSetupTableLogin : public PDUHeader
{
  // Originated from client

  string_t username_;
  string_t password_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(string_t); }

  virtual u_byte_t* write(char* buffer) const
  {
    PDU_DEBUG("--- Send: Table Login PDU ---");

    u_byte_t* pbyte = PDUHeader::write(buffer);

#ifdef PDU_DEBUG_
    CString s;
    CString n = MakeString(username_),
            p = MakeString(password_);
    s.Format("   username: %s password: %s", (LPCSTR)n, (LPCSTR)p);
    PDU_DEBUG((LPCSTR)s);
#endif

    // Write username & password
    memcpy(pbyte, username_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;
    memcpy(pbyte, password_, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;

    return pbyte;
  }

  PDUSetupTableLogin(const char* username,
                     const char* password)
    :
    PDUHeader(PDU_Setup_TableLogin, sizeOf(),
              username, password)
  {
    if (username != NULL && *username != '\0')
      strncpy(username_, username, PDU_STRINGSIZE);
    else
      memset(username_, 0, sizeof(username_));

    if (password != NULL && *password != '\0')
      strncpy(password_, password, PDU_STRINGSIZE);
    else
      memset(password_, 0, PDU_STRINGSIZE);
  }

  PDUSetupTableLogin(const char* buf)
    :
    PDUHeader(buf)
  {
    strncpy(username_, &buf[PDU_HEADER_SIZE], PDU_STRINGSIZE);
    strncpy(password_, &buf[PDU_HEADER_SIZE + PDU_STRINGSIZE], PDU_STRINGSIZE);
  }  
};


struct PDUSetupTableAccept : public PDUHeader
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

  PDUSetupTableAccept(const char* username,
                      const char* password,
                      const char* msg, int msglen)
    :
    PDUHeader(PDU_Setup_TableAccept,
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

  PDUSetupTableAccept(const char* buf)
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

  ~PDUSetupTableAccept()
  {
    delete [] message_;
    message_ = 0;
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Table Accept PDU ---");
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


struct PDUSetupTableReject : public PDUHeader
{
  // originated from server

  void execute(GameState*);

  PDUSetupTableReject(const char* username,
                      const char* password)
    :
    PDUHeader(PDU_Setup_TableReject, sizeOf(),
              username, password)
  {}

  PDUSetupTableReject(const char* buf)
    :
    PDUHeader(buf)
  {}  

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Table Reject PDU ---");
    PDUHeader::checkValid();
  }
};


struct PDUSetupBuyInQuery : public PDUHeader
{
  CChips limit_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(CChips::chip_amount_t); }

/*
  u_byte_t* write(char* buf) const
  {    
    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)PDUHeader::write(buf);
    *pchips++ = limit_.htonl_rep();
    return (u_byte_t*)pchips;
  }

  PDUSetupBuyInQuery(const char* username,
                     const char* password,
                     const CChips& limit)
    :
    PDUHeader(PDU_Setup_BuyInQuery,
              sizeOf(),
              username, password),
    limit_(limit)
  {}
*/

  PDUSetupBuyInQuery(const char* buf)
    :
    PDUHeader(buf),
    limit_   (0)
  {
    PDU_ASSERT(length_ == sizeOf());

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)&buf[PDU_HEADER_SIZE];
    limit_ = CChips_n(*pchips++).ntohl();
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Buy In Query PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   limit: %f", limit_.asDouble());
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


struct PDUSetupBuyInRequest : public PDUHeader
{
  CChips chips_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(CChips::chip_amount_t); }

  u_byte_t* write(char* buf) const
  {    
#ifdef PDU_DEBUG_
    PDU_DEBUG("--- Send: Buy In Request PDU ---");
#endif
    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)PDUHeader::write(buf);
    *pchips++ = chips_.htonl_rep();
    return (u_byte_t*)pchips;
  }

  PDUSetupBuyInRequest(const char* username,
                       const char* password,
                       const CChips& chips)
    :
    PDUHeader(PDU_Setup_BuyInRequest,
              sizeOf(),
              username, password),
    chips_(chips)
  {}

  PDUSetupBuyInRequest(const char* buf)
    :
    PDUHeader(buf),
    chips_   (0)
  {
    PDU_ASSERT(length_ == sizeOf());

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)&buf[PDU_HEADER_SIZE];
    chips_ = CChips_n(*pchips++).ntohl();
  }

};


struct PDUSetupTableLogout : public PDUHeader
{
  // originated by client
  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Table Logout PDU ---");
#endif
    u_byte_t* p = PDUHeader::write(buffer);
    return p;
  }

  PDUSetupTableLogout(const char* username,
                      const char* password)
    :
    PDUHeader(PDU_Setup_TableLogout,
              sizeOf(),
              username, password)
  {}

  PDUSetupTableLogout(const char* buf)
    :
    PDUHeader(buf)
  {}  
};

struct PDUSetupTableLogoutConfirm : public PDUHeader
{
  // originated from server

  void execute(GameState*);

  PDUSetupTableLogoutConfirm(const char* username,
                             const char* password)
    :
    PDUHeader(PDU_Setup_TableLogoutConfirm,
              sizeOf(),
              username, password)
  {}

  PDUSetupTableLogoutConfirm(const char* buf)
    :
    PDUHeader(buf)
  {}  

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Table Logout Confirm PDU ---");
    PDUHeader::checkValid();
  }
};


END_NAMESPACE(Network)


#endif