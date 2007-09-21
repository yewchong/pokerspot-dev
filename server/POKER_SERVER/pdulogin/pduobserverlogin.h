#ifndef __table_pduobserverlogin_h__
#define __table_pduobserverlogin_h__

#include "../pduhandler/pduhandler.h"

// this is identical to login pdu
struct pdu_observerlogin
{
    char username[16];
    char password[16];
};


class CpduObserverLogin : public Cpdu
{
public:
    CpduObserverLogin();
	const char* getUsername() const;
    const char* getPassword() const;
    void grok(SOCKET, CTable*);

    // since user names and passwords are allowed to
    // be 16 characters, we need a 17 byte array to 
    // make it into a 0-terminated string.
    char sz_username[17];
    char sz_password[17];
};

inline const char* CpduObserverLogin::getPassword() const
{
    return sz_password;
};

inline const char* CpduObserverLogin::getUsername() const
{
    return sz_username;
};


#endif
