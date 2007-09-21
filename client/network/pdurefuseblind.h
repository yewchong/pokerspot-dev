#ifndef __network_pdurefuseblind_h__
#define __network_pdurefuseblind_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDURefuseBlind
  Created  : 05/09/2000

  OVERVIEW : Refuse Blind PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDURefuseBlind : public PDUHeader
{
  u_int16_t sequencenumber_;// id for this event

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  // originated by client
  virtual u_byte_t* write(char* buffer) const
  {
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);
    *pint++ = htons(sequencenumber_);
#ifdef PDU_DEBUG_
    PDU_DEBUG("--- Send: Refuse Blind PDU ---");
#endif
    return (u_byte_t*)pint;
  }

  PDURefuseBlind(const char* username,
                 const char* password,
                 u_int16_t   sequencenumber)
    :
    PDUHeader(PDU_Play_RefuseBlind,
              sizeOf(), username, password),
    sequencenumber_(sequencenumber)
  {}

  PDURefuseBlind(const char* buf)
    :
    PDUHeader(buf),
    sequencenumber_(0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    sequencenumber_ = ntohs(*pint++);
  }
};


END_NAMESPACE(Network)

#endif