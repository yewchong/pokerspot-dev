#include "psi_win32.h"
#include "def.h"
#include "sync/sync.h"
#include <set>

#ifdef _WIN32
using namespace std;
#endif

// Allow players from the same ip address to
// log in multiple times
#define NO_IPADDRESS_CHECK 1

// IP Address tracking:
// IP addresses of people currently logged in are stored
// into this set. An ip address is not allowed to connect
// more than once.
set<u_int32_t> g_IPAddresses;

// Protect access from main and game logic thread
// with MUTEX.
Mutex g_ipaddr_mutex;


// Return true if ip address already registered.
bool FindIPAddress(u_int32_t ipaddr)
{
#ifdef NO_IPADDRESS_CHECK
  #ifndef _WIN32
//  this code not for production
  #endif
    return false;
#else

    SingleLock l(&g_ipaddr_mutex);
    if (!l.lock())
        return true;

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

    SingleLock l(&g_ipaddr_mutex);
    if (!l.lock())
        return false;

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

    SingleLock l(&g_ipaddr_mutex);
    if (!l.lock())
        return false;

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
