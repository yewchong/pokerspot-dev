#ifndef __table_pduloginreject_h__
#define __table_pduloginreject_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoginReject
  Created  : 04/18/2000

  OVERVIEW : Login reject pdu. Table server does not allow login retries
             because the user has been authenticated already by the lounge
             server.

 ****************************************************************************/

#include "../pduhandler/pduhandler.h"

class CpduLoginReject : public Cpdu
{
public:
    CpduLoginReject();
    int sendReject(SOCKET);
};


#endif