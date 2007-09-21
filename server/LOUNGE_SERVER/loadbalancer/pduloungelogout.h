#ifndef __lounge_pduloungelogout_h__
#define __lounge_pduloungelogout_h__


/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeLogout
  Created  : 09/26/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer when
             it is shutting down.

 ****************************************************************************/



#include "pdu/pdu.h"


class CpduLoungeLogout : public Cpdu
{
public:
  CpduLoungeLogout();
  int sendLoungeLogout(SOCKET sd);
};

#endif
