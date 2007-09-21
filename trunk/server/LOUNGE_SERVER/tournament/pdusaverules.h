#ifndef __lounge_pdusaverules_h__
#define __lounge_pdusaverules_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSaveRules
  Created  : 08/29/2000

  OVERVIEW : 

 ****************************************************************************/

#include "pdu/pdu.h"


class CpduSaveRules : public Cpdu
{
public:
    CpduSaveRules();
    void grok(SOCKET, CPlayer*);
};


#endif
