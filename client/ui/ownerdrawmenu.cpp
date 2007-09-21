/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class COwnerDrawMenu
  Created  : 06/07/2000

  OVERVIEW : Class for owner drawn menus.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/ownerdrawmenu.h"

COwnerDrawMenu::COwnerDrawMenu()
{
}

COwnerDrawMenu::~COwnerDrawMenu()
{
}

void COwnerDrawMenu::MeasureItem(LPMEASUREITEMSTRUCT lpMIS)
{
    CDC dc;
    dc.CreateCompatibleDC(NULL);
    CFont f;
    f.CreateStockObject(DEFAULT_GUI_FONT);

    CFont* pOldFont = dc.SelectObject(&f);
    CRect r(0, 0, 0, 0);
    dc.DrawText("Game", &r, DT_CALCRECT);
    dc.SelectObject(pOldFont);

    lpMIS->itemWidth = r.Width();
    lpMIS->itemHeight = r.Height();
}

void COwnerDrawMenu::DrawItem(LPDRAWITEMSTRUCT lpDIS)
{
	CDC* pDC = CDC::FromHandle(lpDIS->hDC);
    const CString s("&Game");

    static CRect rectText(0, 0, 0, 0); 
    if (rectText.IsRectEmpty())
    {
        pDC->DrawText(s, &rectText, DT_CALCRECT);
    }

    int x = GetSystemMetrics(SM_CXFRAME) + 5;
    int y = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CYFRAME);
    CRect r(x, y,
            x + rectText.Width(),
            y + rectText.Height());

    int h = GetSystemMetrics(SM_CYMENU);
    if (h > r.Height())
    {   // center to menu 
        r.OffsetRect(0, (h - r.Height()) / 2);
    }

    pDC->DrawText(s, &r, DT_CENTER|DT_VCENTER);

    //    CMenu::DrawItem(lpDIS);
}


