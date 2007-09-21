#ifndef __psi_loginentry_h__
#define __psi_loginentry_h__

#include "../def.h"
#include <map>
#include <string>
USING_NS_T(std, map);
USING_NS_T(std, string);
//#pragma warning( disable : 4786 )


//
// Upon receiving Player Login Query pdu from
// lounge server, save an entry that records
// the player's name and time of the login query.
//
// The server checks at every tick the time labels
// of login entries and removes entries older than
// the timeout value.
//
// A player is allowed to log in only if there
// is a valid login entry at the time he attempts
// the login.
//

struct LoginEntry
{
    LoginEntry(long timer, u_int32_t ipaddr)
        : timer_(timer), ipaddr_(ipaddr)
    {}
    long timer_;
    u_int32_t ipaddr_;
};

// Associate user name with a login entry
typedef map<string, LoginEntry> LoginEntries;

// Login must occur within LOGIN_EXPIRE_TIME
// seconds from login query
#define LOGIN_EXPIRE_TIME 200


#endif
