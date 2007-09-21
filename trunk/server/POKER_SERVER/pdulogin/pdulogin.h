#ifndef __pduLogin_h__
#define __pduLogin_h__

#include "../pduhandler/pduhandler.h"

typedef struct pdu_login {
        char username[16];
        char password[16];
} pdu_login_t;


class CpduLogin: public Cpdu
{
public:
    CpduLogin();
	CpduLogin(Cpdu *);
	const char *getUsername() const;
    const char *getPassword() const;
    void grok(SOCKET, CTable*);

    // since user names and passwords are allowed to
    // be 16 characters, we need a 17 byte array to 
    // make it into a 0-terminated string.
    char sz_username[17];
    char sz_password[17];
};

inline const char *CpduLogin::getPassword() const
{
    return sz_password;
/*
	pdu_login_t *loginPtr = (pdu_login_t *)(value_ + PDU_HEADERSIZE);
	return loginPtr->password;
*/
};

inline const char *CpduLogin::getUsername() const
{
    return sz_username;
/*
	pdu_login_t *loginPtr = (pdu_login_t *)(value_ + PDU_HEADERSIZE);
	return loginPtr->username;
*/
};


#endif
