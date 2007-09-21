#ifndef __lounge_autoupgrade_h__
#define __lounge_autoupgrade_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class AutoUpgrade
  Created  : 04/23/2000

  OVERVIEW : Auto-upgrade related functionality.

 ****************************************************************************/

#include "psi_win32.h"
#include "includes.h"

class AutoUpgrade
{
public:
    enum
    {
        ChecksumSize = 16
    };

    static const char* GetConfigFile(u_int16_t platform);
    static u_byte_t* ComputeChecksum(const char* configFile);
};

#endif