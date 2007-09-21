#ifndef __lounge_pdureseatnack_h__
#define __lounge_pdureseatnack_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduReseatNack
  Created  : 07/26/2000

  OVERVIEW : Receive-only tournament reseat ack pdu.
             Table server tells lounge server that a player
             reseat failed.
 ****************************************************************************/

#include "pdu/pdu.h"
#include "psi_win32.h"


class CpduReseatNack : public Cpdu
{
public:
    CpduReseatNack();

	const char* getUsername() const;
    void grok(SOCKET, CTable*);

private:
    // since user names and passwords are allowed to
    // be 16 characters, we need a 17 byte array to 
    // make it into a 0-terminated string.
    char sz_username[PDU_STRINGSIZE+1];
};

inline const char* CpduReseatNack::getUsername() const
{
    return sz_username;
};



#endif
