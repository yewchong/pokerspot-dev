/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Registry
  Created  : 25.10.1999

  OVERVIEW : Class Registry implements registry access.

 ****************************************************************************/

#include "stdafx.h"
#include "registry.h"
#include "base/registry.h"

namespace
{

LPCSTR g_szAppKey = _T("Software\\PokerSpot International\\Lounge\\Version 1.0\\Settings");

// All registry entries go under HKCU\Software\PokerSpot International

Base::Registry g_Reg(HKEY_CURRENT_USER, g_szAppKey);
                    
}


int Registry::GetBettingTime(int defValue)
{
  return g_Reg.getProfileInt("BettingTime", defValue);
}


int Registry::GetDiscardTime(int defValue)
{
  return g_Reg.getProfileInt("DiscardTime", defValue);
}


CRect Registry::GetWndRect(const CRect& def)
{
  CRect r;
  r.left = g_Reg.getProfileInt("FrameWndLeft", def.left);
  r.top = g_Reg.getProfileInt("FrameWndTop", def.top);
  r.right = g_Reg.getProfileInt("FrameWndRight", def.right);
  r.bottom = g_Reg.getProfileInt("FrameWndBottom", def.bottom);
  return r;
}


LONG Registry::SetWndRect(const CRect& rect)
{
  LONG rc = g_Reg.setProfileInt("FrameWndLeft", rect.left);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("FrameWndTop", rect.top);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("FrameWndRight", rect.right);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("FrameWndBottom", rect.bottom);
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


CPoint Registry::GetActionBarPos(const CPoint& def)
{
  CPoint pos;
  pos.x = g_Reg.getProfileInt("ActionBarPosX", def.x);
  pos.y = g_Reg.getProfileInt("ActionBarPosY", def.y);
  return pos;
}


LONG Registry::SetActionBarPos(const CPoint& pos)
{
  LONG rc = g_Reg.setProfileInt("ActionBarPosX", pos.x);
  if (rc == ERROR_SUCCESS)
    rc = g_Reg.setProfileInt("ActionBarPosY", pos.y);
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
  // decrypt
  return CString(s.c_str());
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
