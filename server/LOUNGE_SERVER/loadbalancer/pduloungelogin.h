#ifndef __lounge_pduloungelogin_h__
#define __lounge_pduloungelogin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeLogin
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             start-up.

 ****************************************************************************/


#include "pdu/pdu.h"


class CpduLoungeLogin : public Cpdu
{
public:
  CpduLoungeLogin();
  int sendLoungeLogin(SOCKET sd, u_int16_t port);
};

#endif
