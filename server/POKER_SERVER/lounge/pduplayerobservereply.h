#ifndef __table_pduplayerobservereply_h__
#define __table_pduplayerobservereply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveReply
  Created  : 05/27/2000

  OVERVIEW : Send-only Player Observe Reply PDU - table server replies
             whether it is ok for a player to observe the table.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

enum ObserveReply
{
  OR_Reject = 0,
  OR_Accept = 1
};
    

class CpduPlayerObserveReply : public Cpdu
{
public:
  CpduPlayerObserveReply();
  int sendObserveReply(SOCKET sd,
                       u_int16_t tableNumber,
                       const char* username,
                       ObserveReply);
};



#endif