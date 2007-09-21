#ifndef __lounge_pduplayerunseated_h__
#define __lounge_pduplayerunseated_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerUnseated
  Created  : 07/07/2000

  OVERVIEW : Receive-only tournament player unseated pdu.
             Table server tells lounge server that a player
             has been logged off from the table.

 ****************************************************************************/

#include "pdu/pdu.h"
#include "psi_win32.h"
#include "tournament/pduplayerseated.h"


class CpduPlayerUnseated : public Cpdu
{
public:
    CpduPlayerUnseated();

	const char* getUsername() const;
    void grok(SOCKET, CTable*);

private:
    // since user names and passwords are allowed to
    // be 16 characters, we need a 17 byte array to 
    // make it into a 0-terminated string.
    char sz_username[PDU_STRINGSIZE+1];
};

inline const char* CpduPlayerUnseated::getUsername() const
{
    return sz_username;
};



#endif
