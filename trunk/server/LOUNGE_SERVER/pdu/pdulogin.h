#ifndef __lounge_pdulogin_h__
#define __lounge_pdulogin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLogin
  Created  : 04/17/2000

  OVERVIEW : Login PDU.

 ****************************************************************************/

#include "pdu.h"
#include "psi_win32.h"

struct pdu_login
{
    char username[PDU_STRINGSIZE];
    char password[PDU_STRINGSIZE];
};

#define PDU_CHECKSUMSIZE 16

struct pdu_version
{
    u_int16_t platform;
    u_byte_t  checksum[PDU_CHECKSUMSIZE];
};


class CpduLogin : public Cpdu
{
public:
    CpduLogin();

	const char* getUsername() const;
    const char* getPassword() const;
    u_int16_t   getPlatform() const;
    u_byte_t*   getChecksum() const;

    void grok(SOCKET, CPlayer*);

private:
    // since user names and passwords are allowed to
    // be 16 characters, we need a 17 byte array to 
    // make it into a 0-terminated string.
    char sz_username[PDU_STRINGSIZE+1];
    char sz_password[PDU_STRINGSIZE+1];
};

inline const char* CpduLogin::getPassword() const
{
    return sz_password;
};

inline const char* CpduLogin::getUsername() const
{
    return sz_username;
};

inline u_int16_t CpduLogin::getPlatform() const
{
	pdu_version* pdu = (pdu_version*)(value_ +
                                      PDU_HEADERSIZE +
                                      sizeof(pdu_login));
	return ntohs(pdu->platform);
}

inline u_byte_t* CpduLogin::getChecksum() const
{
	pdu_version* pdu = (pdu_version*)(value_ +
                                      PDU_HEADERSIZE +
                                      sizeof(pdu_login));
	return pdu->checksum;
}



#endif
