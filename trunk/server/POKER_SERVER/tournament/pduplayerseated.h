#ifndef __table_pduplayerseated_h__
#define __table_pduplayerseated_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerSeated
  Created  : 07/10/2000

  OVERVIEW : Send-only player seated pdu.
             Table server tells lounge server that a player was seated.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

struct pdu_playerseated
{
    char username[PDU_STRINGSIZE];
};

class CpduPlayerSeated : public Cpdu
{
public:
    CpduPlayerSeated();
    int sendPlayerSeated(SOCKET      sd,
                         const char* username);
};




#endif