#ifndef __lounge_pduwaitinglistconfig_h__
#define __lounge_pduwaitinglistconfig_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduWaitinglistConfig
  Created  : 10/06/2001

  OVERVIEW : This class is used by the Load Balancer to send
             information about waiting lists to Lounge Servers.

 ****************************************************************************/

#include "pdu/pdu.h"


struct pdu_waitinglistheader
{
  u_int16_t numEntries; // # of pdu_loungeentries to follow
  u_int16_t operation;  // Add, Remove, SetOwner
};


struct pdu_waitinglistowner
{
  u_int16_t number;
  u_int16_t owner;
};


struct pdu_waitinglistdata
{
  u_int16_t number;
  u_int16_t owner;
  u_int32_t lowlimit;
  u_int32_t highlimit;
};



class CpduWaitinglistConfig : public Cpdu
{
public:
  enum
  {
    Add      = 1,
    Remove   = 2,
    SetOwner = 3
  };

public:
	CpduWaitinglistConfig();
  void grok(SOCKET sd, CLoadBalancer*);
};


#endif