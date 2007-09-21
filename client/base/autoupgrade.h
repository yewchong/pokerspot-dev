#ifndef __base_autoupgrade_h__
#define __base_autoupgrade_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : autoupgrade.h
  Created  : 02/24/2000

  OVERVIEW : Auto-Upgrade related stuff.

 ****************************************************************************/

#include <list>
#include "network/network.h"

namespace Base
{

struct ConfigEntry
{
  CString  module_;
  u_byte_t checksum_[16];
  CString  url_;
  DWORD    size_;

  ConfigEntry()
    : size_(0)
  {
    memset(checksum_, 0, sizeof(checksum_));
  }
  ConfigEntry(LPCSTR module, u_byte_t* checksum, LPCSTR url, DWORD size)
    : module_(module), url_(url), size_(size)
  {
    memcpy(checksum_, checksum, sizeof(checksum_)); 
  }
  ConfigEntry(const ConfigEntry& ce)
    : module_(ce.module_),
      url_   (ce.url_),
      size_  (ce.size_)
  {
    memcpy(checksum_, ce.checksum_, sizeof(checksum_)); 
  }
  ConfigEntry& operator = (const ConfigEntry& rhs)
  {
    if (this != &rhs)
    {
      module_ = rhs.module_;
      memcpy(checksum_, rhs.checksum_, sizeof(checksum_)); 
      url_ = rhs.url_;
      size_ = rhs.size_;
    }
    return *this;
  }
};

inline
bool operator == (const ConfigEntry& lhs, const ConfigEntry& rhs)
{
  return lhs.module_ == rhs.module_ &&
         memcmp(lhs.checksum_, rhs.checksum_, sizeof(lhs.checksum_)) == 0;
}

typedef std::list<ConfigEntry> ConfigEntries;

/*
 * FUNCTION: int ReadConfigurationFile
 *
 * PURPOSE: read configuration file 'fname', and return
 *          configuration entries in 'entries'.
 */
extern
int ReadConfigurationFile(const char* fname, ConfigEntries& entries);

/*
 * FUNCTION: int GetUpgradeModules
 *
 * PURPOSE: read configuration file 'fname', and return
 *          a configuration in 'entries' for each module
 *          that needs an upgrade.
 */
extern
int GetUpgradeModules(const char* fname, ConfigEntries& need_upgrade);

} // namespace

#endif