#ifndef __cwg_ipfunc_h__
#define __cwg_ipfunc_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : registeripaddress.h
  Created  : 04/16/2000

  OVERVIEW : Functions for registering/unregistering an ip address
             so as to prevent the same ip address from logging in
             more than once.

 ****************************************************************************/

#include "compat.h"
#include "psi_win32.h"

namespace CWG_IpFunc
{
  extern bool FindIPAddress(u_int32_t);
  extern bool RegisterIPAddress(u_int32_t);
  extern bool UnregisterIPAddress(u_int32_t);
}


#endif