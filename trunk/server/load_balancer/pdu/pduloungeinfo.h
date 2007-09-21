#ifndef __lb_pduloungeinfo_h__
#define __lb_pduloungeinfo_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduLoungeInfo
  Created  : 09/27/2001

  OVERVIEW : This class is used by the Load Balancer to send

             - currently connected lounges a new lounge
             - new lounge to already connected lounges

             when a new Lounge Server logs in to the Load Balancer.

             This will effectively let every Lounge Server to
             know of every other Lounge Server, creating a fully
             connected mesh.

 ****************************************************************************/

#include "pdu/pdu.h"


struct pdu_loungeinfo
{
  u_int16_t numEntries; // # of pdu_loungeentries to follow
  u_int16_t operation;  // Add, Remove or SetNumber
};

struct pdu_loungeentry
{
  u_int16_t number;
  u_int16_t port;
  u_int32_t ipaddress;
};


class CpduLoungeInfo : public CPDU
{
public:
  enum
  {
    Add       = 1,
    Remove    = 2,
    SetNumber = 3
  };

public:
	CpduLoungeInfo();

	int sendNewLoungeInfo(CLounge* newLounge);
  int sendRemoveLoungeInfo(CLounge* removedLounge);
};

#endif