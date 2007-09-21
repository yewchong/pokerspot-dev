#ifndef __lounge_pduloginreject_h__
#define __lounge_pduloginreject_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoginReject
  Created  : 04/18/2000

  OVERVIEW : Login reject pdu.

 ****************************************************************************/


#include "pdu.h"

enum RejectFlag
{
    RF_Allow_Retry = 1,
    RF_No_Retry    = 2
};

class CpduLoginReject : public Cpdu
{
public:
    CpduLoginReject();
    int sendReject(SOCKET, RejectFlag);
};


#endif