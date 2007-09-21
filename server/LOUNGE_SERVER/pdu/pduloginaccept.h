#ifndef __lounge_pduloginaccept_h__
#define __lounge_pduloginaccept_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduLoginAccept
  Created  : 04/18/2000

  OVERVIEW : Login accept pdu.

 ****************************************************************************/


#include "pdu.h"

class CpduLoginAccept : public Cpdu
{
public:
    CpduLoginAccept(const char* username,
                    const char* password,
                    const char* motd);
};


#endif