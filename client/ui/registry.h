#ifndef __psi_registry_h__
#define __psi_registry_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Registry
  Created  : 25.10.1999

  OVERVIEW : Class Registry implements registry access.

 ****************************************************************************/


class Registry
{
public:
  static void SetInstanceNumber(int);
  static int  GetInstanceNumber();
  static int  GetTournamentTest(int);

  static int GetUseSSL(int defValue);
  static int Get1_02(int defValue);
  static LONG Set1_02(int value);
  static int GetActionBarLayout(int defValue);
  static LONG SetActionBarLayout(int value);
  static int GetActionBarDock(int defValue);
  static LONG SetActionBarDock(int value);
  static int GetChatWndDock(int defValue);
  static LONG SetChatWndDock(int value);
  static int GetSoundEnabled(int defValue);
  static LONG SetSoundEnabled(int value);
  static int GetActionBarTTips(int defValue);
  static LONG SetActionBarTTips(int value);
  static CString GetHandLogEmailAddress(const CString& defVal);
  static LONG SetHandLogEmailAddress(const CString& value);
  static int GetCheckMuckHand(int defValue);
  static LONG SetCheckMuckHand(int value);
  static int GetSettingsTakeFocus(int defValue);
  static LONG SetSettingsTakeFocus(int value);

  // Saving & loading of main frame pos &
  // dimensions
  static CRect  GetWndRect(const CRect& defValue);
  static LONG   SetWndRect(const CRect& value);
  static CPoint GetActionBarPos(const CPoint& defValue);
  static LONG   SetActionBarPos(const CPoint& value);
  static CRect  GetChatRect(const CRect& defValue);
  static LONG   SetChatRect(const CRect& value);
  static int    GetSplitterPos(int defValue);
  static LONG   SetSplitterPos(int value);

  static DWORD GetDealCardTime(int defValue);
  static DWORD GetShowAllCards(int defValue);
  static DWORD GetDrawPlayerBounds(int defValue);
  static DWORD GetWaitActionTime(int defValue);
  static DWORD GetShowdownTime(int defValue);

  static CString GetServerHostName(const CString& def);
  static int     GetServerPort(int defVal);
  static LONG    SetServerHostName(const CString&);
  static LONG    SetServerPort(int);

  static int     GetUpgradeServerPort(int defVal);

  static CString GetUsername(const CString& def);
  static CString GetPassword(const CString& def);
  static LONG    SetUsername(const CString&);
  static LONG    SetPassword(const CString&);
};


#endif


