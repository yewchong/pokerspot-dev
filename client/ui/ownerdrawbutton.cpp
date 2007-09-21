/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class COwnerDrawButton
  Created  : 01/31/2000

  OVERVIEW : COwnerDrawButton for the lounge client.

 ****************************************************************************/


#include "stdafx.h"
#include "ownerdrawbutton.h"
#include "global.h"
#include "ui/resource.h"
#include "base/draw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawButton

CBrush  COwnerDrawButton::brHilite_;
CFont   COwnerDrawButton::fontHilite_;

COwnerDrawButton::COwnerDrawButton()
  :
  hilite_(FALSE)
{
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


COwnerDrawButton::~COwnerDrawButton()
{
}


BEGIN_MESSAGE_MAP(COwnerDrawButton, CButton)
  //{{AFX_MSG_MAP(COwnerDrawButton)
  ON_WM_MEASUREITEM()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COwnerDrawButton message handlers

void COwnerDrawButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
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

  const COLORREF clrLight = RGB(0, 220, 0),
                 clrFace = RGB(0, 128, 0),
                 clrDark = RGB(0, 64, 0);

  CBrush brFace;
  brFace.CreateSolidBrush(clrFace);
  pDC->FillRect(&lpDrawItemStruct->rcItem, &brFace);

  if (!isPressed)
  {
    Base::DrawBumpRect(pDC, &r, clrDark, clrLight);  
    r.InflateRect(1, 1);
    Base::DrawBumpRect(pDC, &r, clrDark, clrLight);  
  }
  else
  {
    Base::DrawBumpRect(pDC, &r, clrLight, clrDark);
    r.InflateRect(1, 1);
    Base::DrawBumpRect(pDC, &r, clrLight, clrDark);
  }

  CString text;
  GetWindowText(text);

  if (text.GetLength() > 0)
  {
    pDC->SetBkMode(TRANSPARENT);

    if (isPressed)
      r.OffsetRect(1, 1);
    Base::DrawBumpText(pDC, text, r,
                       DT_CENTER|DT_SINGLELINE|DT_VCENTER,
                       clrLight, clrDark);

/*
    COLORREF colorText = ::GetSysColor(COLOR_MENUTEXT);
    if (isDisabled)
      colorText = ::GetSysColor(COLOR_GRAYTEXT);

    if (isDisabled)
    {
      Base::DrawBumpText(pDC, text, r,
                         DT_CENTER|DT_SINGLELINE|DT_VCENTER,
                         colorText, ::GetSysColor(COLOR_3DHILIGHT));
    }
    else
    {
      pDC->SetTextColor(colorText);
      pDC->DrawText(text, text.GetLength(),
                    &lpDrawItemStruct->rcItem,
                    DT_CENTER|DT_SINGLELINE|DT_VCENTER);
    }
*/
  }

  if (pOldFont)
    pDC->SelectObject(pOldFont);
}


void COwnerDrawButton::OnMeasureItem(int nIDCtl, LPMEASUREITEMSTRUCT lpMeasureItemStruct) 
{
  const int BtnSizeX = 60;
  const int BtnSizeY = int(1.5f * float(Global::GetCharHeight()));

  lpMeasureItemStruct->itemWidth = BtnSizeX;
  lpMeasureItemStruct->itemHeight = BtnSizeY;

  CButton::OnMeasureItem(nIDCtl, lpMeasureItemStruct);
}
