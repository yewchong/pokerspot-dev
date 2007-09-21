/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CActionButton
  Created  : 25.10.1999

  OVERVIEW : CActionButton implements one button in the action bar.

 ****************************************************************************/


#include "stdafx.h"
#include "actionbutton.h"
#include "global.h"
#include "ui/resource.h"
#include "base/draw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActionButton

CBrush  CActionButton::brHilite_;
CFont   CActionButton::fontHilite_;
Base::TransparentBitmap CActionButton::bmp_;

CActionButton::CActionButton()
  :
  hilite_(FALSE)
{
  if (bmp_.GetSize() == CSize(0, 0))
  {
    BOOL rc = bmp_.LoadBitmap(IDB_CHECKBTN, IDB_CHECKBTNMASK);
    ASSERT(rc);
  }

  if (!brHilite_.GetSafeHandle())
    brHilite_.CreateSolidBrush(RGB(0, 128, 0));

  if (!fontHilite_.GetSafeHandle())
  {
    CFont* pFont = 0;//Global::GetFont();
    if (pFont)
    {
      LOGFONT lf;
      pFont->GetLogFont(&lf);
      fontHilite_.CreateFontIndirect(&lf);
    }
  }
}


CActionButton::~CActionButton()
{
}


BEGIN_MESSAGE_MAP(CActionButton, CButton)
  //{{AFX_MSG_MAP(CActionButton)
  ON_WM_MEASUREITEM()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActionButton message handlers

void CActionButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  if (!pDC) return;    

  CFont* pOldFont = 0;
  if (hilite_ && fontHilite_.GetSafeHandle())
    pOldFont = pDC->SelectObject(&fontHilite_);
  else
    pOldFont = (CFont*)pDC->SelectStockObject(DEFAULT_GUI_FONT);

  UINT isDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);
  UINT isPressed = (GetState() & 0x0004);

  CRect r(lpDrawItemStruct->rcItem);
  r.InflateRect(-1, -1);

  if (!isPressed)
    Base::DrawBumpRect(pDC, lpDrawItemStruct->rcItem,
                       RGB(10, 10, 10), RGB(240, 240, 240));  
  else
    Base::DrawBumpRect(pDC, lpDrawItemStruct->rcItem,
                       RGB(240, 240, 240), RGB(10, 10, 10));

  CString text;
  GetWindowText(text);

  if (text.GetLength() > 0)
  {
    pDC->SetBkMode(TRANSPARENT);

    COLORREF colorText = ::GetSysColor(COLOR_MENUTEXT);
    if (isDisabled)
      colorText = ::GetSysColor(COLOR_GRAYTEXT);

    if (isDisabled)
    {
      Base::DrawBumpText(pDC, text, r,
                         DT_CENTER|DT_SINGLELINE|DT_VCENTER,
                         colorText, ::GetSysColor(COLOR_3DHILIGHT));
    }
    else if (hilite_)
    { // Render two spade bitmaps on the button
      CSize s = bmp_.GetSize();
      bmp_.TransBlt(pDC, 2, (r.bottom / 2) - (s.cy / 2));
      bmp_.TransBlt(pDC, r.right - s.cx - 2, (r.bottom / 2) - (s.cy / 2));
      pDC->SetTextColor(colorText);
      pDC->DrawText(text, text.GetLength(),
                    &lpDrawItemStruct->rcItem,
                    DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    }
    else
    {
      pDC->SetTextColor(colorText);
      pDC->DrawText(text, text.GetLength(),
                    &lpDrawItemStruct->rcItem,
                    DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    }
  }

  if (pOldFont)
    pDC->SelectObject(pOldFont);
}


void CActionButton::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
  const int BtnSizeX = 60;
  const int BtnSizeY = int(1.5f * float(Global::GetCharHeight()));

  lpMeasureItemStruct->itemWidth = BtnSizeX;
  lpMeasureItemStruct->itemHeight = BtnSizeY;

  CButton::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
