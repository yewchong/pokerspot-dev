#ifndef __lb_pduplayerloggedout_h__
#define __lb_pduplayerloggedout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerLoggedOut
  Created  : 10/04/2001

  OVERVIEW : Lounge Server sends this PDU to the Load Balancer upon
             a player logout/disconnect.

 ****************************************************************************/


#include "pdu/pdu.h"


class CpduPlayerLoggedOut : public CPDU
{
public:
  CpduPlayerLoggedOut();
  
  const char* getUsername() const;  
  void grok(SOCKET, CLounge*);
};


#endif
