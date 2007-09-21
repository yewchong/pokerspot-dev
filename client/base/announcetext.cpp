/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : announcetext.cpp
  Created  : 01/26/2000

  OVERVIEW : Routines for drawing announcement text.

 ****************************************************************************/

#include "stdafx.h"
#include "base/announcetext.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Base
{

CRect SizeAnnounceText(CDC* pDC, const CString& text, const CRect& rect)
{
  CRect r(rect);
  r.InflateRect(-2, -4);
  pDC->DrawText(text, text.GetLength(), &r, DT_CALCRECT|DT_WORDBREAK);
  return r;
}

void DrawAnnounceText(CDC* pDC, const CString& text, const CRect& rect)
{
  CRect r(rect);
  r.InflateRect(-2, -4);
  pDC->DrawText(text, text.GetLength(), &r, DT_WORDBREAK);
}

}

