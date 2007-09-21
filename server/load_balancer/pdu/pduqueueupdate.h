#ifndef __lb_pduqueueupdate_h__
#define __lb_pduqueueupdate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduQueueUpdate
  Created  : 10/08/2001

  OVERVIEW : This PDU is used for broadcasting changes
             in the waiting lists.

 ****************************************************************************/


#include "pdu/pdu.h"


class CpduQueueUpdate : public CPDU
{
public:
  CpduQueueUpdate();
  void grok(SOCKET, CLounge*);
};


#endif