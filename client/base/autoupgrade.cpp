/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : autoupgrade.cpp
  Created  : 02/24/2000

  OVERVIEW : Auto-Upgrade related stuff.

 ****************************************************************************/

#include "stdafx.h"
#include "base/autoupgrade.h"
#include "base/md5.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Base
{

/*
 * FUNCTION: int ReadConfigurationFile
 *
 * PURPOSE: read configuration file 'fname', and return
 *          configuration entries in 'entries'.
 */
int ReadConfigurationFile(const char* fname, ConfigEntries& entries)
{
  char* module = NULL;
  char* url = NULL;
  char* asciisum = NULL;
  char* downloadsize = NULL;

  // 16-byte MD5 sum
  u_byte_t checksum[16];
  memset(checksum, 0, sizeof(checksum));
  char atobin[3];
  atobin[2] = '\0';

  const int BufSize = 512;
  char readbuff[BufSize];	
  FILE* versionfd = fopen(fname, "ra");
  if (!versionfd)
  {
    fprintf(stderr, "Can't open version info file: clientversion.conf.\n");
    return -1;
  }

  while (fgets(readbuff, BufSize, versionfd))
  { 
    module = strtok(readbuff, "$");
    asciisum = strtok(NULL, "$");
    url = strtok(NULL, "$");
    downloadsize = strtok(NULL, "$");

    // Convert ascii checksum to binary
    if (asciisum != NULL && strlen(asciisum) == 32)
    { 
      for (int i = 0; i < 16; i++)
      {
        atobin[0] = asciisum[2 * i];
        atobin[1] = asciisum[2 * i + 1];
        checksum[i] = (unsigned char)strtol(atobin, (char**)NULL, 16);
      }
    }

    if (url != NULL)
    {
      entries.push_back(ConfigEntry(module,
                                    checksum,
                                    url,
                                    atoi(downloadsize)));
    }
  }

  fclose(versionfd);
  return 0;
}


/*
 * FUNCTION: int GetUpgradeModules
 *
 * PURPOSE: read configuration file 'fname', and return
 *          a configuration in 'entries' for each module
 *          that needs an upgrade.
 *
 *          Read current configuration file, compare checksums
 *          against files on disk. For each mismatch, add a
 *          config entry to 'entries".
 */
int GetUpgradeModules(const char* fname, ConfigEntries& need_upgrade)
{
  BOOL rc = TRUE;

  // Read the configuration file
  ConfigEntries currentConfig;
  if (ReadConfigurationFile("client_versions.conf", currentConfig) == -1)
  {
    CString s;
    s.Format("Configuration file 'client_versions.conf' was not found. It must reside in the PokerSpot client installation directory.");
    AfxMessageBox(s);
    return FALSE;
  }

  // Compare it to the modules on disk
  for (ConfigEntries::const_iterator it = currentConfig.begin(),
                                     end = currentConfig.end();
       it != end;
       ++it)
  {
    u_byte_t checksum[16];
    memset(checksum, 0, sizeof(checksum));
    Base::ComputeMD5Checksum(it->module_, checksum);

    if (memcmp(checksum, it->checksum_, sizeof(checksum)) != 0)
    {
      need_upgrade.push_back(*it);
    }
  }

  return TRUE;
}

} // namespace