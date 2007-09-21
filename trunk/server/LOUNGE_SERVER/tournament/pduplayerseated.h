#ifndef __lounge_pduplayerseated_h__
#define __lounge_pduplayerseated_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerSeated
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament player seated pdu.
             Table server tells lounge server that a player
             has been seated at the table.

 ****************************************************************************/

#include "pdu/pdu.h"
#include "psi_win32.h"

struct pdu_playerseated
{
    char username[PDU_STRINGSIZE];
};


class CpduPlayerSeated : public Cpdu
{
public:
    CpduPlayerSeated();

	const char* getUsername() const;
    void grok(SOCKET, CTable*);

private:
    // since user names and passwords are allowed to
    // be 16 characters, we need a 17 byte array to 
    // make it into a 0-terminated string.
    char sz_username[PDU_STRINGSIZE+1];
};

inline const char* CpduPlayerSeated::getUsername() const
{
    return sz_username;
};



#endif
