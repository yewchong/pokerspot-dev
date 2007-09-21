/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Registry
  Created  : 25.10.1999

  OVERVIEW : Class Registry implements basic registry functionality.

 ****************************************************************************/

#include "stdafx.h"
#include "base/registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
  const int MaxString = 512;

  void OutputLastError()
  {
    LPVOID lpMsgBuf = 0;

    ::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | 
                      FORMAT_MESSAGE_FROM_SYSTEM |    
                      FORMAT_MESSAGE_IGNORE_INSERTS,   
                    NULL,
                    GetLastError(),
                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
                    (LPTSTR)&lpMsgBuf,
                    0,
                    NULL );// Process any inserts in lpMsgBuf.

    CString s;
    s.Format("Error: %s\n", (LPSTR)lpMsgBuf);
    OutputDebugString((LPCSTR)s);

    LocalFree(lpMsgBuf);
  }

}


namespace Base
{

using std::string;

Registry::Registry(HKEY hRoot, LPCSTR appKey)
  :
  hRoot_ (hRoot),
  appKey_(appKey)
{
}


Registry::~Registry()
{
}


string Registry::getProfileString(LPCSTR valueName, LPCSTR defaultValue)
{
  string s(defaultValue);  
  HKEY hKey = 0;
  LONG result = ::RegOpenKeyEx(hRoot_,
                               appKey_.c_str(),
                               0, KEY_READ, &hKey);

  if (result == ERROR_SUCCESS)
  {
    TCHAR str[MaxString];
    DWORD type = 0,
          size = MaxString;

    result = ::RegQueryValueEx(hKey,
                               valueName,
                               NULL, &type,
                               (BYTE*)str, &size);

    ::RegCloseKey(hKey);
    hKey = 0;
 
    if (result == ERROR_SUCCESS)
      s = str;
  }

  return s;
}


int Registry::getProfileInt(LPCSTR valueName, int defaultValue)
{
  int i = defaultValue;  
  HKEY hKey = 0;
  LONG result = ::RegOpenKeyEx(hRoot_,
                               appKey_.c_str(),
                               0, KEY_READ, &hKey);

  if (result == ERROR_SUCCESS)
  {
    DWORD type = 0,
          size = sizeof(int);

    result = ::RegQueryValueEx(hKey,
                               valueName,
                               NULL, &type,
                               (BYTE*)&i, &size);

    ::RegCloseKey(hKey);
    hKey = 0;
   }

  return i;
}


bool Registry::getProfileBool(LPCSTR valueName, bool defaultValue)
{
  bool b = defaultValue;  
  HKEY hKey = 0;
  LONG result = ::RegOpenKeyEx(hRoot_,
                               appKey_.c_str(),
                               0, KEY_READ, &hKey);

  if (result == ERROR_SUCCESS)
  {
    DWORD type = 0,
          size = sizeof(bool);

    result = ::RegQueryValueEx(hKey,
                               valueName,
                               NULL, &type,
                               (BYTE*)&b, &size);

    ::RegCloseKey(hKey);
    hKey = 0;
   }

  return b;
}


LONG Registry::setProfileString(LPCSTR valueName, LPCSTR value)
{
  HKEY hKey = 0;
  LONG result = ::RegOpenKeyEx(hRoot_,
                               appKey_.c_str(),
                               0, KEY_WRITE, &hKey);

  if (result == ERROR_FILE_NOT_FOUND)
    result = ::RegCreateKey(hRoot_, appKey_.c_str(), &hKey);

  if (result == ERROR_SUCCESS)
  {
    result = ::RegSetValueEx(hKey,
                             valueName,
                             0,
                             REG_SZ,
                             (const BYTE*)value,
                             (lstrlen(value)+1) * sizeof(CHAR));

#ifdef _DEBUG
    if (result != ERROR_SUCCESS)
      OutputLastError();
#endif

    ::RegCloseKey(hKey);
    hKey = 0;
  }

  return result;
}


LONG Registry::setProfileInt(LPCSTR valueName, int value)
{
  HKEY hKey = 0;
  LONG result = ::RegOpenKeyEx(hRoot_,
                               appKey_.c_str(),
                               0, KEY_WRITE|KEY_SET_VALUE, &hKey);

  if (result == ERROR_FILE_NOT_FOUND)
    result = ::RegCreateKey(hRoot_, appKey_.c_str(), &hKey);

  if (result == ERROR_SUCCESS)
  {
    DWORD v = value;
    result = ::RegSetValueEx(hKey,
                             valueName,
                             0,
                             REG_DWORD,
                             (const BYTE*)&v,
                             sizeof(DWORD));

#ifdef _DEBUG
    if (result != ERROR_SUCCESS)
      OutputLastError();
#endif

    ::RegCloseKey(hKey);
    hKey = 0;
  }

  return result;
}


void Registry::enumKeys(LPCSTR subKey, KeyEnumCallback cbFunc, VOID* pvContext)
{
  ASSERT(cbFunc != NULL);

  HKEY hKey1 = NULL;

  string s = appKey_ + string("\\") + subKey;

  LONG result = ::RegOpenKeyEx(hRoot_,
                               s.c_str(),
                               0, KEY_READ, &hKey1);

  if (hKey1 != NULL)
  { 
    LONG rc = ERROR_SUCCESS;
    for (DWORD i = 0; rc == ERROR_SUCCESS; i++)
    {
      TCHAR buf[_MAX_PATH];
      DWORD size = _MAX_PATH;

      rc = ::RegEnumKeyEx(hKey1, i,
                          buf, &size,
                          NULL,
                          NULL, 0,
                          NULL);

      if (rc == ERROR_SUCCESS &&
          size > 0 && size < _MAX_PATH)
      {
        buf[size] = '\0';

        if (!(cbFunc(hRoot_, s.c_str(), buf, pvContext)))
          rc = ERROR_NO_MORE_ITEMS;
      }                          
    }
  }

  if (hKey1)
    ::RegCloseKey(hKey1);
}

} 
