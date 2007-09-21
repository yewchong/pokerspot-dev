#ifndef __psi_ipcheck_h__
#define __psi_ipcheck_h__

#include "psi_win32.h"

extern bool FindIPAddress(u_int32_t ipaddr);
extern bool RegisterIPAddress(u_int32_t ipaddr);
extern bool UnregisterIPAddress(u_int32_t ipaddr);


#endif