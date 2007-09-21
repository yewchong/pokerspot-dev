#ifndef __lounge_pduunknown_h__
#define __lounge_pduunknown_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : CpduUnknown
  Created  : 04/16/2000

  OVERVIEW : PDU class that handles incoming unknown pdus.

 ****************************************************************************/



#include "pdu.h"

class CpduUnknown : public Cpdu
{
public:
    CpduUnknown();
    void grok(SOCKET);
};


#endif