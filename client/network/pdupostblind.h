#ifndef __network_pdupostblind_h__
#define __network_pdupostblind_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPostBlind
  Created  : 12.11.1999

  OVERVIEW : Post Blind Out PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUPostBlind : public PDUHeader
{
  u_int16_t sequencenumber_;// id for this event

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  // originated by client
  virtual u_byte_t* write(char* buffer) const
  {
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);
    *pint++ = htons(sequencenumber_);

    PDU_DEBUG("--- Send: Blind Post PDU ---");

    return (u_byte_t*)pint;
  }

  PDUPostBlind(const char* username,
               const char* password,
               u_int16_t   sequencenumber)
    :
    PDUHeader(PDU_Play_PostBlind,
              sizeOf(), username, password),
    sequencenumber_(sequencenumber)
  {}

  PDUPostBlind(const char* buf)
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