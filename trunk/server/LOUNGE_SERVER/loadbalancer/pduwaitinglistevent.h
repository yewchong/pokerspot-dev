#ifndef __lounge_pduwaitinglistevent_h__
#define __lounge_pduwaitinglistevent_h__

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
#include <string>

class CLoadBalancer;
class CLoungeProxy;
class CTournament;


struct pdu_waitinglisteventheader
{
  u_int16_t queueNumber;
  u_int16_t reason;
};


// Tournament events are sent with this structure
struct pdu_tournamentevent
{
  u_int16_t number;
};


// Table events are sent with this structure
struct pdu_tableevent
{
  u_int16_t number;
  u_int32_t ipaddress;
  u_int16_t port;
};



class CpduWaitinglistEvent : public Cpdu
{
public:
  enum {
    WLE_Player_Add        = 1,
    WLE_Player_Remove     = 2,
    WLE_Player_Serviced   = 3,
    WLE_Table_Add         = 4,
    WLE_Table_Remove      = 5,
    WLE_Tournament_Add    = 6,
    WLE_Tournament_Remove = 7
  };


public:
  CpduWaitinglistEvent();
 
  int sendAdd(u_int16_t queuenumber, CTable* table);
  int sendRemove(u_int16_t queuenumber, CTable* table);

  int sendAdd(u_int16_t queuenumber, CTournament* tournament);
  int sendRemove(u_int16_t queuenumber, CTournament* tournament);

  void grok(SOCKET sd, CLoadBalancer*);
  void grok(SOCKET sd, CLoungeProxy*);
  
private:
  int sendDelta(u_int16_t queuenumber,
                CTable* table,
                u_int16_t reason);  
  int sendDelta(u_int16_t queuenumber,
                CTournament* tournament,
                u_int16_t reason);
};

#endif