#ifndef __network_pduloungehandshake_h__
#define __network_pduloungehandshake_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : CPDULoungeHandshake
  Created  : 09/30/2001

  OVERVIEW : This PDU does the initial handshake with the
             Load Balancer server.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


class CPDULoungeHandshake : public PDUHeader
{
  string_t username_;
  u_int16_t platform_;
  u_byte_t checksum_[CLIENT_CHECKSUM_SIZE];

public:
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(string_t) +
                                 sizeof(u_int16_t) +                                  
                                 CLIENT_CHECKSUM_SIZE; }

  // Originated from client
  virtual u_byte_t* write(char* buffer) const;

  CPDULoungeHandshake(const char* username,
                      u_int16_t   platform,
                      u_byte_t*   checksum);
};


END_NAMESPACE(Network)

#endif