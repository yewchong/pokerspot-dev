#ifndef __lounge_pdusaveformat_h__
#define __lounge_pdusaveformat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSaveFormat
  Created  : 08/29/2000

  OVERVIEW : 

 ****************************************************************************/

#include "pdu/pdu.h"


class CpduSaveFormat : public Cpdu
{
public:
    CpduSaveFormat();
    void grok(SOCKET, CPlayer*);
};


#endif
