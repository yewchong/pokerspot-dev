/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class TabCtrl
  Created  : 02/03/2000

  OVERVIEW : TabCtrl is an owner-draw subclass of CTabCtrl.

 ****************************************************************************/

#include "stdafx.h"
#include "tabctrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(TabCtrl, CTabCtrl)
  //{{AFX_MSG_MAP(TabCtrl)
  ON_WM_ERASEBKGND()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()


TabCtrl::TabCtrl()
{}

void TabCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{}

BOOL TabCtrl::OnEraseBkgnd(CDC* pDC)
{
/*
    CRect r;
    GetClientRect(&r);
    CBrush br;
    //br.CreateSolidBrush(::GetSysColor(COLOR_BTNFACE));
    br.CreateSolidBrush(RGB(255, 255, 255));
    pDC->FillRect(&r, &br);
    return TRUE;
*/
    return FALSE;
}