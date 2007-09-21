#ifndef __base_draw_h__
#define __base_draw_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : Namespace Base
  Created  : 25.10.1999

  OVERVIEW : Misc helper drawing functions.

 ****************************************************************************/


namespace Base
{
  // Draw fake 3D text at 'pos'.
  extern void DrawBumpText(CDC* pDC,
                           LPCSTR pszText,
                           const CPoint& pos,
                           const COLORREF& clrLight,
                           const COLORREF& clrDark);

  // Draw fake 3D text at 'rect'; nFormat
  // is the same as in the DrawText API.
  extern void DrawBumpText(CDC* pDC,
                           LPCSTR pszText,
                           CRect& rect,
                           UINT  nFormat,
                           const COLORREF& clrLight,
                           const COLORREF& clrDark);

  // Draw fake 3D text at 'rect' with three colors;
  // nFormat is the same as in the DrawText API.
  extern void DrawBumpText(CDC* pDC,
                           LPCSTR pszText,
                           CRect& rect,
                           UINT  nFormat,
                           const COLORREF& clrLight,
                           const COLORREF& clrBase,
                           const COLORREF& clrDark);

  // Draw a bump rectangle.
  extern void DrawBumpRect(CDC* pDC,
                           const CRect& rect,
                           const COLORREF& clrLight,
                           const COLORREF& clrDark);


  extern void DrawRectAlpha(CDC* pDC,
                            const CRect& rect,
                            BYTE r, BYTE g, BYTE b, BYTE a);

}

#endif

