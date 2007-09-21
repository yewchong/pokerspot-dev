#ifndef __network_pdusitout_h__
#define __network_pdusitout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUSitOut
  Created  : 12.11.1999

  OVERVIEW : Sit Out PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUSitOut : public PDUHeader
{
  // originated by client
  virtual u_byte_t* write(char* buffer) const
  {
    u_byte_t* p = PDUHeader::write(buffer);
#ifdef PDU_DEBUG_
    PDU_DEBUG("--- Send: Sit Out PDU ---");
#endif
    return p;
  }

  PDUSitOut(const char* username,
            const char* password)
    :
    PDUHeader(PDU_Play_SitOut,
              sizeOf(), username, password)
  {}

  PDUSitOut(const char* buf)
    :
    PDUHeader(buf)
  {}
};


END_NAMESPACE(Network)

#endif