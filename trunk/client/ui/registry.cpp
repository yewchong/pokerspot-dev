/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Registry
  Created  : 25.10.1999

  OVERVIEW : Class Registry implements registry access.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/registry.h"
#include "base/registry.h"

namespace
{

LPCSTR g_szAppKey = _T("Software\\PokerSpot International\\Table\\Version 1.02\\Settings");

// All registry entries go under HKCU\Software\PokerSpot International

Base::Registry g_Reg(HKEY_CURRENT_USER, g_szAppKey);
int g_InstanceNumber = 0;
                    
}

void Registry::SetInstanceNumber(int n)
{
    g_InstanceNumber = n;
}

int Registry::GetInstanceNumber()
{
    return g_InstanceNumber;
}


int Registry::GetTournamentTest(int defV)
{
  return g_Reg.getProfileInt("TournamentTest", defV);
}


int Registry::GetUseSSL(int defValue)
{
  return g_Reg.getProfileInt("EnableSSL", defValue);
}


int Registry::GetActionBarLayout(int defValue)
{
  CString k;
  k.Format("ActionBarLayout-%d", g_InstanceNumber);
  return g_Reg.getProfileInt(k, defValue);
}


LONG Registry::SetActionBarLayout(int value)
{
  CString k;
  k.Format("ActionBarLayout-%d", g_InstanceNumber);
  return g_Reg.setProfileInt(k, value);
}

int Registry::GetActionBarDock(int defValue)
{
  CString k;
  k.Format("ActionBarDock-%d", g_InstanceNumber);
  return g_Reg.getProfileInt(k, defValue);
}


LONG Registry::SetActionBarDock(int value)
{
  CString k;
  k.Format("ActionBarDock-%d", g_InstanceNumber);
  return g_Reg.setProfileInt(k, value);
}

int Registry::GetChatWndDock(int defValue)
{
  CString k;
  k.Format("ChatWndDock-%d", g_InstanceNumber);
  return g_Reg.getProfileInt(k, defValue);
}

LONG Registry::SetChatWndDock(int value)
{
  CString k;
  k.Format("ChatWndDock-%d", g_InstanceNumber);
  return g_Reg.setProfileInt(k, value);
}


int Registry::GetSoundEnabled(int defValue)
{
  return g_Reg.getProfileInt("SoundEnabled", defValue);
}


LONG Registry::SetSoundEnabled(int value)
{
  return g_Reg.setProfileInt("SoundEnabled", value);
}


int Registry::GetActionBarTTips(int defValue)
{
  return g_Reg.getProfileInt("ActionBarTTips", defValue);
}


LONG Registry::SetActionBarTTips(int value)
{
  return g_Reg.setProfileInt("ActionBarTTips", value);
}


int Registry::GetCheckMuckHand(int defValue)
{
  return g_Reg.getProfileInt("CheckMuckHand", defValue);
}


LONG Registry::SetCheckMuckHand(int value)
{
  return g_Reg.setProfileInt("CheckMuckHand", value);
}


int Registry::GetSettingsTakeFocus(int defValue)
{
  return g_Reg.getProfileInt("TakeFocusOnRequest", defValue);
}


LONG Registry::SetSettingsTakeFocus(int value)
{
  return g_Reg.setProfileInt("TakeFocusOnRequest", value);
}


CString Registry::GetHandLogEmailAddress(const CString& defVal)
{
  std::string s = g_Reg.getProfileString("EmailAddress", defVal);
  return s.c_str();
}


LONG Registry::SetHandLogEmailAddress(const CString& value)
{
  return g_Reg.setProfileString("EmailAddress", value);
}



CRect Registry::GetWndRect(const CRect& def)
{
  CString l, t, r, b;
  l.Format("FrameWndLeft-%d", g_InstanceNumber);
  t.Format("FrameWndTop-%d", g_InstanceNumber);
  r.Format("FrameWndRight-%d", g_InstanceNumber);
  b.Format("FrameWndBottom-%d", g_InstanceNumber);

  CRect rect;
  rect.left = g_Reg.getProfileInt(l, def.left);
  rect.top = g_Reg.getProfileInt(t, def.top);
  rect.right = g_Reg.getProfileInt(r, def.right);
  rect.bottom = g_Reg.getProfileInt(b, def.bottom);

  if (rect.left < 0)
      rect.left = 0;
  if (rect.top < 0)
      rect.top = 0;
  if (rect.right > GetSystemMetrics(SM_CXSCREEN))
      rect.right = GetSystemMetrics(SM_CXSCREEN);
  if (rect.bottom > GetSystemMetrics(SM_CYSCREEN))
      rect.bottom = GetSystemMetrics(SM_CYSCREEN);

  return rect;
}


LONG Registry::SetWndRect(const CRect& rect)
{
  CString l, t, r, b;
  l.Format("FrameWndLeft-%d", g_InstanceNumber);
  t.Format("FrameWndTop-%d", g_InstanceNumber);
  r.Format("FrameWndRight-%d", g_InstanceNumber);
  b.Format("FrameWndBottom-%d", g_InstanceNumber);

  LONG rc = g_Reg.setProfileInt(l, rect.left);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt(t, rect.top);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt(r, rect.right);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt(b, rect.bottom);
  return rc;
}


CRect Registry::GetChatRect(const CRect& def)
{
  CString l, t, r, b;
  l.Format("ChatWndLeft-%d", g_InstanceNumber);
  t.Format("ChatWndTop-%d", g_InstanceNumber);
  r.Format("ChatWndRight-%d", g_InstanceNumber);
  b.Format("ChatWndBottom-%d", g_InstanceNumber);

  CRect rect;
  rect.left = g_Reg.getProfileInt(l, def.left);
  rect.top = g_Reg.getProfileInt(t, def.top);
  rect.right = g_Reg.getProfileInt(r, def.right);
  rect.bottom = g_Reg.getProfileInt(b, def.bottom);

  if (rect.left < 0)
      rect.left = 0;
  if (rect.top < 0)
      rect.top = 0;
  if (rect.right > GetSystemMetrics(SM_CXSCREEN))
      rect.right = GetSystemMetrics(SM_CXSCREEN);
  if (rect.bottom > GetSystemMetrics(SM_CYSCREEN))
      rect.bottom = GetSystemMetrics(SM_CYSCREEN);

  return rect;
}


LONG Registry::SetChatRect(const CRect& rect)
{
  CString l, t, r, b;
  l.Format("ChatWndLeft-%d", g_InstanceNumber);
  t.Format("ChatWndTop-%d", g_InstanceNumber);
  r.Format("ChatWndRight-%d", g_InstanceNumber);
  b.Format("ChatWndBottom-%d", g_InstanceNumber);

  LONG rc = g_Reg.setProfileInt(l, rect.left);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt(t, rect.top);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt(r, rect.right);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt(b, rect.bottom);
  return rc;
 
}


CPoint Registry::GetActionBarPos(const CPoint& def)
{
  CString x, y;
  x.Format("ActionBarPosX-%d", g_InstanceNumber);
  y.Format("ActionBarPosY-%d", g_InstanceNumber);

  CPoint pos;
  pos.x = g_Reg.getProfileInt(x, def.x);
  pos.y = g_Reg.getProfileInt(y, def.y);

  if (pos.x < 0)
      pos.x = 0;
  if (pos.y < 0)
      pos.y = 0;
  if (pos.x > GetSystemMetrics(SM_CXSCREEN))
      pos.x = GetSystemMetrics(SM_CXSCREEN);
  if (pos.y > GetSystemMetrics(SM_CYSCREEN))
      pos.y = GetSystemMetrics(SM_CYSCREEN);

  return pos;
}


LONG Registry::SetActionBarPos(const CPoint& pos)
{
  CString x, y;
  x.Format("ActionBarPosX-%d", g_InstanceNumber);
  y.Format("ActionBarPosy-%d", g_InstanceNumber);

  LONG rc = g_Reg.setProfileInt(x, pos.x);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt(y, pos.y);
  return rc;
}


int Registry::GetSplitterPos(int defValue)
{
  return g_Reg.getProfileInt("SplitterPos", defValue);
}


LONG Registry::SetSplitterPos(int val)
{
  return g_Reg.setProfileInt("SplitterPos", val);
}


DWORD Registry::GetDealCardTime(int defValue)
{
  return g_Reg.getProfileInt("DealCardTime", defValue);
}


DWORD Registry::GetShowAllCards(int defValue)
{
  return g_Reg.getProfileInt("ShowAllCards", defValue);
}


DWORD Registry::GetDrawPlayerBounds(int defValue)
{
  return g_Reg.getProfileInt("DrawPlayerBounds", defValue);
}


DWORD Registry::GetShowdownTime(int defValue)
{
  return g_Reg.getProfileInt("ShowdownTime", defValue);
}


DWORD Registry::GetWaitActionTime(int defValue)
{
  return g_Reg.getProfileInt("WaitActionTime", defValue);
}


CString Registry::GetServerHostName(const CString& defVal)
{
  std::string s = g_Reg.getProfileString("ServerHost",
                                         (LPCSTR)defVal);
  return s.c_str();
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


int Registry::GetUpgradeServerPort(int defVal)
{
  return g_Reg.getProfileInt("UpgradeServerPort", defVal);
}

CString Registry::GetUsername(const CString& def)
{
  std::string s = g_Reg.getProfileString("Username",
                                         (LPCSTR)def);
  return s.c_str();
}


CString Registry::GetPassword(const CString& def)
{
  std::string s = g_Reg.getProfileString("Password",
                                         (LPCSTR)def);
  // decrypt
  return s.c_str();
}


LONG Registry::SetUsername(const CString& s)
{
  return g_Reg.setProfileString("Username", s);
}


LONG Registry::SetPassword(const CString& s)
{
  // encrypt
  return g_Reg.setProfileString("Password", s);
}


int Registry::Get1_02(int defValue)
{
    return g_Reg.getProfileInt("1_02", defValue);
}

LONG Registry::Set1_02(int value)
{
    return g_Reg.setProfileInt("1_02", value);
}
