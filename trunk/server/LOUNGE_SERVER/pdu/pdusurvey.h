#ifndef __lounge_pdusurvey_h__
#define __lounge_pdusurvey_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSurvey
  Created  : 04/18/2000

  OVERVIEW : Lounge survey PDU.

 ****************************************************************************/

#include "pdu.h"


class CpduSurvey : public Cpdu
{
public:
    CpduSurvey();
    void grok(SOCKET, CPlayer*);
};


#endif
