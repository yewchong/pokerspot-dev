#ifndef __network_pduredirectlogin_h__
#define __network_pduredirectlogin_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : class CPDURedirectLogin
  Created  : 09/29/2001

  OVERVIEW : Load Balancer sends this PDU to the Lounge Client
             to redirect him to the appropriate Lounge Server.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

// Originated from server

class CPDURedirectLogin : public PDUHeader
{
  u_int32_t host_;
  u_int16_t port_;

public:
  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int32_t) +
                                 sizeof(u_int16_t); }

  CPDURedirectLogin(const char* buf);
};


END_NAMESPACE(Network)


#endif