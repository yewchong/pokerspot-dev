#ifndef __network_pduping_h__
#define __network_pduping_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUPing, PDUPong
  Created  : 02/08/2000

  OVERVIEW : Ping & Pong PDU.

             Upon receiving a Ping PDU, the client immediately
             sends a Pong PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUPing : public PDUHeader
{
  u_int32_t data_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int32_t); }

  PDUPing(const char* buf)
    :
    PDUHeader(buf),
    data_    (0)
  {
    u_int32_t* pint = (u_int32_t*)&buf[PDU_HEADER_SIZE];
    data_ = ntohl(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Ping PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   data: %d", data_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


struct PDUPong : public PDUHeader
{
  u_int32_t data_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int32_t); }

  PDUPong(const char* username,
          const char* password,
          u_int32_t   data)
    :
    PDUHeader(PDU_Pong, sizeOf(),
              username, password),
    data_(data)
  {}

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Pong PDU ---");
    u_int32_t* pint = (u_int32_t*)PDUHeader::write(buf);
    *pint++ = htonl(data_);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   data: %d ", data_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }

};


END_NAMESPACE(Network)

#endif