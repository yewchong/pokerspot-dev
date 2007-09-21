#ifndef __network_pdugamenumber_h__
#define __network_pdugamenumber_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUGameNumber
  Created  : 02/10/2000

  OVERVIEW : Game Number PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUGameNumber : public PDUHeader
{
  // Originated from server
  u_int32_t gamenumber_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                          sizeof(u_int32_t); }

  virtual u_byte_t* write(char* buffer) const
  {
    u_int32_t* plong = (u_int32_t*)PDUHeader::write(buffer);
    *plong++ = htonl(gamenumber_);
    return (u_byte_t*)plong;
  }

  PDUGameNumber(const char* username,
                const char* password,
                u_int32_t   gamenumber)
    :
    PDUHeader  (PDU_GameNumber, sizeOf(),
                username, password),
    gamenumber_(gamenumber)
  {
  }

  PDUGameNumber(const char* buf)
    :
    PDUHeader  (buf),
    gamenumber_(0)
  {
    u_int32_t* plong = (u_int32_t*)&buf[PDU_HEADER_SIZE];
    gamenumber_ = ntohl(*plong++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Game Number PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   number: %ld", gamenumber_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif