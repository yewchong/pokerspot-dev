#ifndef __psi_loungeregistry_h__
#define __psi_loungeregistry_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Registry
  Created  : 02/25/2000

  OVERVIEW : Class Registry implements registry access
             for the lounge client.

 ****************************************************************************/


class Registry
{
public:
  static int GetUseSSL(int defValue);

  static CString GetUpgradeWizard(const CString& def);
  static int     GetAutoUpgradeEnabled(int);

  static LONG    SetUpgradeWizard(const CString&);

  static int Get1_02(int defValue);
  static LONG Set1_02(int value);

  static int GetLoginToPlayMoney(int);
  static LONG SetLoginToPlayMoney(int);

  // Saving & loading of main frame pos &
  // dimensions
  static CRect  GetWndRect(const CRect& defValue);
  static LONG   SetWndRect(const CRect& value);
  static CRect  GetChatRect(const CRect& defValue);
  static LONG   SetChatRect(const CRect& value);
  static CRect  GetTableDetailsRect(const CRect& defValue);
  static LONG   SetTableDetailsRect(const CRect& value);

  static CString GetServerHostName(const CString& def);
  static int     GetServerPort(int defVal);
  static LONG    SetServerHostName(const CString&);
  static LONG    SetServerPort(int);

  static CString GetUsername(const CString& def);
  static CString GetPassword(const CString& def);
  static LONG    SetUsername(const CString&);
  static LONG    SetPassword(const CString&);
  static int     GetRememberNamePw(int defVal);
  static LONG    SetRememberNamePw(int);
  static CString GetPlayMoneyHost(const CString& def);
  static int     GetPlayMoneyPort(int defVal);
};


#endif


