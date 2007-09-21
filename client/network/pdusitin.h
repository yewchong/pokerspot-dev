#ifndef __network_pdusitin_h__
#define __network_pdusitin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUSitIn
  Created  : 12.11.1999

  OVERVIEW : Sit In PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUSitIn : public PDUHeader
{
  u_byte_t* write(char* buf) const
  {    
    u_byte_t* pbyte = PDUHeader::write(buf);
#ifdef PDU_DEBUG_
    PDU_DEBUG("--- Send: Sit In PDU ---");
#endif
    return pbyte;
  }

  PDUSitIn(const char* username,
           const char* password)
    :
    PDUHeader(PDU_Play_SitIn,
              sizeOf(), username, password)
  {}

  PDUSitIn(const char* buf)
    :
    PDUHeader(buf)
  {}
};


END_NAMESPACE(Network)

#endif