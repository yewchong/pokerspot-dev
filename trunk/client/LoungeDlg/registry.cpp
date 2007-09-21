/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Registry
  Created  : 02/25/2000

  OVERVIEW : Class Registry implements registry access
             for the lounge client.

 ****************************************************************************/

#include "stdafx.h"
#include "registry.h"
#include "base/registry.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
  // All registry entries go under HKCU\Software\PokerSpot International
  LPCSTR g_szAppKey = _T("Software\\PokerSpot International\\Lounge\\Version 1.02\\Settings");
  Base::Registry g_Reg(HKEY_CURRENT_USER, g_szAppKey);                    
}


int Registry::GetUseSSL(int defValue)
{
  return g_Reg.getProfileInt("EnableSSL", defValue);
}


CRect Registry::GetWndRect(const CRect& def)
{
  CRect r;
  r.left = g_Reg.getProfileInt("MainWndLeft", def.left);
  r.top = g_Reg.getProfileInt("MainWndTop", def.top);
  r.right = g_Reg.getProfileInt("MainWndRight", def.right);
  r.bottom = g_Reg.getProfileInt("MainWndBottom", def.bottom);

  if (r.left < 0)
      r.left = 0;
  if (r.top < 0)
      r.top = 0;
  if (r.right > GetSystemMetrics(SM_CXSCREEN))
      r.right = GetSystemMetrics(SM_CXSCREEN);
  if (r.bottom > GetSystemMetrics(SM_CYSCREEN))
      r.bottom = GetSystemMetrics(SM_CYSCREEN);

  return r;
}


LONG Registry::SetWndRect(const CRect& rect)
{
  LONG rc = g_Reg.setProfileInt("MainWndLeft", rect.left);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("MainWndTop", rect.top);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("MainWndRight", rect.right);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("MainWndBottom", rect.bottom);
  return rc;
}


CRect Registry::GetChatRect(const CRect& def)
{
  CRect r;
  r.left = g_Reg.getProfileInt("ChatWndLeft", def.left);
  r.top = g_Reg.getProfileInt("ChatWndTop", def.top);
  r.right = g_Reg.getProfileInt("ChatWndRight", def.right);
  r.bottom = g_Reg.getProfileInt("ChatWndBottom", def.bottom);
  return r;
}


LONG Registry::SetChatRect(const CRect& rect)
{
  LONG rc = g_Reg.setProfileInt("ChatWndLeft", rect.left);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("ChatWndTop", rect.top);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("ChatWndRight", rect.right);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("ChatWndBottom", rect.bottom);
  return rc; 
}


CRect Registry::GetTableDetailsRect(const CRect& def)
{
  CRect r;
  r.left = g_Reg.getProfileInt("TableDetailsWndLeft", def.left);
  r.top = g_Reg.getProfileInt("TableDetailsWndTop", def.top);
  r.right = g_Reg.getProfileInt("TableDetailsWndRight", def.right);
  r.bottom = g_Reg.getProfileInt("TableDetailsWndBottom", def.bottom);

  if (r.left < 0)
      r.left = 0;
  if (r.top < 0)
      r.top = 0;
  if (r.right > GetSystemMetrics(SM_CXSCREEN))
      r.right = GetSystemMetrics(SM_CXSCREEN);
  if (r.bottom > GetSystemMetrics(SM_CYSCREEN))
      r.bottom = GetSystemMetrics(SM_CYSCREEN);

  return r;
}


LONG Registry::SetTableDetailsRect(const CRect& rect)
{
  LONG rc = g_Reg.setProfileInt("TableDetailsWndLeft", rect.left);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("TableDetailsWndTop", rect.top);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("TableDetailsWndRight", rect.right);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("TableDetailsWndBottom", rect.bottom);
  return rc;
}


CString Registry::GetServerHostName(const CString& defVal)
{
  std::string s = g_Reg.getProfileString("ServerHost",
                                         (LPCSTR)defVal);
  return CString(s.c_str());
}


int Registry::GetServerPort(int defVal)
{
  return g_Reg.getProfileInt("ServerPort", defVal);
}


LONG Registry::SetServerHostName(const CString& s)
{
  return g_Reg.setProfileString("ServerHost", s);
}


LONG Registry::SetServerPort(int p)
{
  return g_Reg.setProfileInt("ServerPort", p);
}


CString Registry::GetUsername(const CString& def)
{
  std::string s = g_Reg.getProfileString("Username",
                                         (LPCSTR)def);
  return CString(s.c_str());
}


CString Registry::GetPassword(const CString& def)
{
  std::string s = g_Reg.getProfileString("Password",
                                         (LPCSTR)def);
  return CString(s.c_str());
}


LONG Registry::SetUsername(const CString& s)
{
  return g_Reg.setProfileString("Username", s);
}


LONG Registry::SetPassword(const CString& s)
{
  return g_Reg.setProfileString("Password", s);
}

int Registry::GetRememberNamePw(int defVal)
{
  return g_Reg.getProfileInt("RememberNamePw", defVal);
}

LONG Registry::SetRememberNamePw(int i)
{
  return g_Reg.setProfileInt("RememberNamePw", i);
}


CString Registry::GetUpgradeWizard(const CString& def)
{
  std::string s = g_Reg.getProfileString("UpgradeWizard",
                                         (LPCSTR)def);
  return CString(s.c_str());
}

int Registry::GetAutoUpgradeEnabled(int i)
{
    return g_Reg.getProfileInt("AutoUpgradeEnabled", 1);
}

LONG Registry::SetUpgradeWizard(const CString& s)
{
  return g_Reg.setProfileString("UpgradeWizard",
                                (LPCSTR)s);
}

int Registry::Get1_02(int defValue)
{
    return g_Reg.getProfileInt("1_02", defValue);
}

LONG Registry::Set1_02(int value)
{
    return g_Reg.setProfileInt("1_02", value);
}

int Registry::GetLoginToPlayMoney(int defValue)
{
    return g_Reg.getProfileInt("PlayMoney", defValue);
}

LONG Registry::SetLoginToPlayMoney(int value)
{
    return g_Reg.setProfileInt("PlayMoney", value);
}


CString Registry::GetPlayMoneyHost(const CString& defVal)
{
  std::string s = g_Reg.getProfileString("PlayMoneyHost",
                                         (LPCSTR)defVal);
  return CString(s.c_str());
}

int Registry::GetPlayMoneyPort(int defValue)
{
    return g_Reg.getProfileInt("PlayMoneyPort", defValue);
}
