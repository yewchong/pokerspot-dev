#ifndef __lb_pdunotify_h__
#define __lb_pdunotify_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduNotify
  Created  : 10/05/2001

  OVERVIEW : Send-only Notify pdu.

 ****************************************************************************/


#include "pdu/pdu.h"


class CpduNotify : public CPDU
{
public:

  enum
  {
    // Client will close connection after
    // displaying message
    CloseConnection = 0,

    // Message will be displayed in status bar
    // instead of a dialog
    StatusMessage = 1,

    // The client will not close the socket
    // upon receiving this PDU
    NoCloseConnection = 2
  };

public:
    CpduNotify();
    int sendNotifyMessage(SOCKET sd, const char* msg, int flag = 0);
};


#endif