#ifndef __table_pduplayerloginreply_h__
#define __table_pduplayerloginreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginReply
  Created  : 04/24/2000

  OVERVIEW : Send-only Player Login Reply PDU - table server replies
             whether it is ok to seat a player in the table.

 ****************************************************************************/

#include "../pduhandler/pduhandler.h"

enum LoginReply
{
    LR_Reject = 0,
    LR_Accept = 1
};
    

class CpduPlayerLoginReply : public Cpdu
{
public:
    CpduPlayerLoginReply();
    int sendLoginReply(SOCKET sd,
                       u_int16_t tableNumber,
                       const char* username,
                       LoginReply);
};



#endif