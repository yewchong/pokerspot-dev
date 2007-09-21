/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class AutoUpgrade
  Created  : 04/23/2000

  OVERVIEW : Auto-upgrade related functionality.

 ****************************************************************************/

#include "upgrade/autoupgrade.h"
#include "upgrade/compute_md5_checksum.h"

const char* AutoUpgrade::GetConfigFile(u_int16_t platform)
{
    static const char* configFiles[] =
    {
      "win_client_versions.conf",
      "mac_client_versions.conf"
    };

    return configFiles[platform];
}

u_byte_t* AutoUpgrade::ComputeChecksum(const char* configFile)
{
    static u_byte_t checksum[ChecksumSize];
    if (compute_md5_checksum(configFile, checksum) != -1)
    {
        return checksum;
    }
    return NULL;
}
