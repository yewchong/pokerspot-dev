#ifndef __base_registry_h__
#define __base_registry_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Registry
  Created  : 25.10.1999

  OVERVIEW : Class Registry implements basic registry functionality.

 ****************************************************************************/


#include <string>

namespace Base
{

using std::string;

typedef BOOL (*KeyEnumCallback)(HKEY hRoot,
                                LPCTSTR appkey,
                                LPCTSTR keyname,
                                VOID*   pvContext);

class Registry
{
public:
  Registry(HKEY root, LPCSTR appKey);
  ~Registry();

public:
  int    getProfileInt(LPCSTR key, int defValue = 0);
  string getProfileString(LPCSTR key, LPCSTR defValue = "");
  bool   getProfileBool(LPCSTR valueName, bool defaultValue = false);

  void   enumKeys(LPCSTR subKey, KeyEnumCallback cbFunc, VOID* pvContext);

  LONG   setProfileInt(LPCSTR key, int value);
  LONG   setProfileString(LPCSTR key, LPCSTR value);
  bool   setProfileBool(LPCSTR valueName, bool value);

private:
  HKEY   hRoot_;
  string appKey_;
};


} // Base

#endif