#ifndef __network_pduhandlogrequest_h__
#define __network_pduhandlogrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUHandLogRequest
  Created  : 03/08/2000

  OVERVIEW : Hand Log Request PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUHandLogRequest : public PDUHeader
{
  // originated by client

  u_int16_t num_hands_;
  u_int16_t addrlen_;
  message_t emailaddr_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t); }

  virtual u_byte_t* write(char* buffer) const;

  PDUHandLogRequest(u_int16_t   num_hands,
                    const char* emailaddr,
                    int         addrlen);

  ~PDUHandLogRequest()
  {
    delete [] emailaddr_;
    emailaddr_ = 0;
  }  
};

END_NAMESPACE(Network)


#endif