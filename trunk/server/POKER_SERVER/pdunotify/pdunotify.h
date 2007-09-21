#ifndef __table_pdunotify_h__
#define __table_pdunotify_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduNotify
  Created  : 04/19/2000

  OVERVIEW : Send-only Notify pdu.

 ****************************************************************************/


#include "../pduhandler/pduhandler.h"

enum NotifyFlag
{
    // Message will be displayed in status bar
    // instead of a window
    NF_StatusMessage = 1,
    // The client will not close the socket
    // upon receiving this PDU
    NF_NoCloseConnection = 2
};

class CpduNotify : public Cpdu
{
public:
    CpduNotify();
    int sendNotifyMessage(SOCKET sd, const char* msg, int flag = 0);
    int sendAllRaw(CTable*, const char* msg, int flag = 0); 
    int sendObservers(CTable*, const char* msg, int flag = 0);
};


#endif