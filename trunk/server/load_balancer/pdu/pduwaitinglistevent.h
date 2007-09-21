#ifndef __lb_pduwaitinglistevent_h__
#define __lb_pduwaitinglistevent_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduWaitinglistEvent
  Created  : 10/02/2001

  OVERVIEW : This PDU is used for sending waiting list events to
             other Lounge Servers.

             The possible waiting list events are:
             - table addition
             - table remove
             - tournament addition
             - tournament remove

 ****************************************************************************/


#include "pdu/pdu.h"


class CpduWaitinglistEvent : public CPDU
{
public:
  CpduWaitinglistEvent();
  void grok(SOCKET sd, CLounge*);
};


#endif