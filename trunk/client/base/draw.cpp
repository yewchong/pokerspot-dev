/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : Namespace Base
  Created  : 25.10.1999

  OVERVIEW : Misc helper drawing functions.

 ****************************************************************************/

#include "stdafx.h"
#include "base/draw.h"
#include "dib/dib.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Base
{

void DrawBumpText(CDC* pDC,
                  LPCSTR pszText,
                  const CPoint& pos,
                  const COLORREF& clrLight,
                  const COLORREF& clrDark)
{
  COLORREF oldColor = pDC->SetTextColor(clrDark);
  pDC->TextOut(pos.x + 1, pos.y + 1, pszText);  

  pDC->SetTextColor(clrLight);
  pDC->TextOut(pos.x, pos.y, pszText);

  pDC->SetTextColor(oldColor);
}


void DrawBumpText(CDC* pDC,
                  LPCSTR pszText,
                  CRect& rect,
                  UINT  nFormat,
                  const COLORREF& clrLight,
                  const COLORREF& clrDark)
{
  CRect r1(rect);
  r1.OffsetRect(1, 1);
  COLORREF oldColor = pDC->SetTextColor(clrDark);
  pDC->DrawText(pszText, lstrlen(pszText), &r1, nFormat);
  
  pDC->SetTextColor(clrLight);
  CRect r2(rect);
  pDC->DrawText(pszText, lstrlen(pszText), &r2, nFormat);

  if (nFormat & DT_CALCRECT)
    rect.UnionRect(r1, r2);

  pDC->SetTextColor(oldColor);
}


void DrawBumpText(CDC* pDC,
                  LPCSTR pszText,
                  CRect& rect,
                  UINT  nFormat,
                  const COLORREF& clrLight,
                  const COLORREF& clrBase,
                  const COLORREF& clrDark)
{
  if (nFormat & DT_CALCRECT)
  { // quick-n-rough size calc
    pDC->DrawText(pszText, lstrlen(pszText), &rect, nFormat);
    return;
  }

  pDC->SetBkMode(TRANSPARENT);
  CRect r2(rect);
  COLORREF oldColor = pDC->SetTextColor(clrLight);
  pDC->DrawText(pszText, lstrlen(pszText), &r2, nFormat);

  CRect r1(rect);
  r1.OffsetRect(2, 2);
  pDC->SetTextColor(clrDark);
  pDC->DrawText(pszText, lstrlen(pszText), &r1, nFormat);
  
  pDC->SetTextColor(clrBase);
  CRect r3(rect);
  r3.OffsetRect(1, 1);
  pDC->DrawText(pszText, lstrlen(pszText), &r3, nFormat);

  pDC->SetTextColor(oldColor);
}



void DrawBumpRect(CDC* pDC,
                  const CRect& rect,
                  const COLORREF& clrLight,
                  const COLORREF& clrDark)
{
  CPen penLight(PS_SOLID, 1, clrLight),
       penDark(PS_SOLID, 1, clrDark);
  CPen* pOldPen = pDC->SelectObject(&penDark);

  pDC->MoveTo(rect.right - 1, rect.top);
  pDC->LineTo(rect.left, rect.top);
  pDC->LineTo(rect.left, rect.bottom - 1);
  pDC->SelectObject(&penLight);
  pDC->LineTo(rect.right - 1, rect.bottom - 1);
  pDC->LineTo(rect.right - 1, rect.top);

  pDC->SelectObject(pOldPen);
}


void DrawRectAlpha(CDC* pDC,
                   const CRect& rect,
                   BYTE r, BYTE g, BYTE b, BYTE a)
{
  CDib dib;
  dib.Create(rect.Width(), rect.Height());
  dib.CopyFromDCRect(pDC,
                     rect.left, rect.top,
                     rect.Width(), rect.Height());

  dib.FillGlass(r, g, b, a);
  dib.CopyToDCRect(pDC, rect.left, rect.top);
}


} // Base


