#ifndef __lounge_pduunknown_h__
#define __lounge_pduunknown_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : CpduUnknown
  Created  : 04/16/2000

  OVERVIEW : PDU class that handles incoming unknown pdus.

 ****************************************************************************/



#include "pdu/pdu.h"

class CpduUnknown : public CPDU
{
public:
  CpduUnknown();
  void grok(SOCKET);
};


#endif