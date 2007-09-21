#ifndef __base_misc_h__
#define __base_misc_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : misc.h
  Created  : 31.10.1999

  OVERVIEW : Misc routines.

 ****************************************************************************/


namespace Base
{
  extern void SafeRemove(const char* filename);

  extern int PointsToPixels(int points);
  extern BOOL IsMaximized(CWnd* pWnd);

  extern CView* GetActiveView();
  extern void SanityCheckMinMax(DWORD& x, DWORD minx, DWORD maxx);

  // center 'r1' inside 'r2', flags = DT_CENTER|DT_VCENTER
  extern void CenterRect(CRect& r1, const CRect& r2, DWORD flags);

  extern void SetStatusText(CWnd* pWnd, LPCTSTR text);

  class Card;
  extern CString AsString(const Card&);
}

#endif