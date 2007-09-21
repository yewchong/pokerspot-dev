#ifndef __lounge_pduupgraderequest_h__
#define __lounge_pduupgraderequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduUpgradeRequest
  Created  : 04/23/2000

  OVERVIEW : Upgrade Request PDU.

             When the lounge server detects that the client has old
             versions of files, it sends Upgrade Request PDU telling 
             the client where to download new versions of the files.

 ****************************************************************************/

#include "pdu.h"

class CpduUpgradeRequest : public Cpdu
{
public:
    CpduUpgradeRequest();
    int sendUpgradeRequest(SOCKET sd,
                           const char* configFile);
};

#endif
