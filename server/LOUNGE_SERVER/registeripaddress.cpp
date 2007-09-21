/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : registeripaddress.cpp
  Created  : 04/16/2000

  OVERVIEW : Functions for registering/unregistering an ip address
             so as to prevent the same ip address from logging in
             more than once.

 ****************************************************************************/

#include <set>
#include "registeripaddress.h"

// Allow same ip address log in multiple times
#ifdef _WIN32
#define NO_IPADDRESS_CHECK 1
#endif

namespace CWG_IpFunc
{

// IP Address tracking:
// IP addresses of people currently logged in are stored
// into this set. An ip address is not allowed to connect
// more than once.
std::set<u_int32_t> g_IPAddresses;

// Return true if ip address already registered.
bool FindIPAddress(u_int32_t ipaddr)
{
#ifdef NO_IPADDRESS_CHECK
    return false;
#else
    return g_IPAddresses.find(ipaddr) != g_IPAddresses.end();
#endif
}

// Registers ip address, returns true if registered,
// false if ip address was already registered.
bool RegisterIPAddress(u_int32_t ipaddr)
{
#ifdef NO_IPADDRESS_CHECK
    return true;
#else
    return (g_IPAddresses.insert(ipaddr).second);
#endif
}

// Unregisters ip address, returns true if unregistered,
// false if ip address was not registered.
bool UnregisterIPAddress(u_int32_t ipaddr)
{
#ifdef NO_IPADDRESS_CHECK
    return true;
#else
    set<u_int32_t>::iterator pos = 
        g_IPAddresses.find(ipaddr);

    if (pos != g_IPAddresses.end())
    {
        g_IPAddresses.erase(pos);
        return true;
    }
    else
    {
        return false;
    }
#endif
}

} // namespace