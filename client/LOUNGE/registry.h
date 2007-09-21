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
  static int GetBettingTime(int defValue);
  static int GetDiscardTime(int defValue);

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

  static CString GetUsername(const CString& def);
  static CString GetPassword(const CString& def);
  static LONG    SetUsername(const CString&);
  static LONG    SetPassword(const CString&);
};


#endif


