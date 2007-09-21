#ifndef __lb_pdurouteloungepdu_h__
#define __lb_pdurouteloungepdu_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduRouteLoungePdu
  Created  : 10/02/2001

  OVERVIEW : Class CpduRouteLoungePdu acts as a wrapper class around other
             PDU objects that need to be routed to a specific Lounge Server.

 ****************************************************************************/

#include "pdu/pdu.h"


struct pdu_routingdata
{
  u_int16_t flag;
  u_int16_t number;
};

struct pdu_username
{
  char username[PDU_STRINGSIZE];
};


class CpduRouteLoungePdu : public CPDU
{
public:
  enum
  {
    WaitinglistNumber = 1,
    TableNumber       = 2
  };

public:
  CpduRouteLoungePdu();
  void grok(SOCKET sd, CLounge*);
};


#endif