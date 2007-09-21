/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class CCheckButton
  Created  : 19.12.1999

  OVERVIEW : CCheckButton implements an owner-drawn check one button.

 ****************************************************************************/

#include "stdafx.h"
#include "ui/checkbutton.h"
#include "ui/resource.h"
#include "ui/actionbar.h"
#include "base/draw.h"

BEGIN_MESSAGE_MAP(CCheckButton, CButton)
  //{{AFX_MSG_MAP(CCheckButton)
  ON_WM_LBUTTONUP()
  //}}AFX_MSG_MAP
END_MESSAGE_MAP()



Base::TransparentBitmap CCheckButton::bmpCheck_;

CCheckButton::CCheckButton()
  :
  isChecked_(0)
{
  if (bmpCheck_.GetSize() == CSize(0, 0))
  {
    BOOL rc = bmpCheck_.LoadBitmap(IDB_CHECK, IDB_CHECKMASK);
    ASSERT(rc);
  }
}

void CCheckButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
  CDC* pDC = CDC::FromHandle(lpDrawItemStruct->hDC);
  if (!pDC) return;

  UINT isDisabled = (lpDrawItemStruct->itemState & ODS_DISABLED);

  CString s;
  GetWindowText(s);
  const int TextXStart = 18;

  //CFont* pOldFont = pDC->SelectObject(CPokerApp::GetFont());

  if (isDisabled)
  {
    int oldMode = pDC->SetBkMode(TRANSPARENT);
    Base::DrawBumpText(pDC, s,
                       CPoint(TextXStart, 0), 
                       ::GetSysColor(COLOR_GRAYTEXT),
                       ::GetSysColor(COLOR_3DHILIGHT));
    pDC->SetBkMode(oldMode);
  }
  else
    pDC->TextOut(TextXStart, 0, s);

  //pDC->SelectObject(pOldFont);

  CRect r(0, 1, 12, 13);
  CBrush br;
  if (isDisabled)
    br.CreateSysColorBrush(COLOR_BTNFACE);
  else
    br.CreateSysColorBrush(COLOR_WINDOW);
    
  pDC->FillRect(&r, &br);
  Base::DrawBumpRect(pDC, r,
                     RGB(255, 255, 255),
                     RGB(0, 0, 0));

  if (isChecked_)
  {
    bmpCheck_.TransBlt(pDC, 1, 2);
  }
}
 
//
// Dispatch button ups manually as button checked
// to the action bar. This is a workaround to the 
// problem that occurs when the check buttons
// are double-clicked and the second click is
// not passed to the dialog by the framework.
//
void CCheckButton::OnLButtonUp(UINT nFlags, CPoint point) 
{
  isChecked_ = !isChecked_;
  CButton::OnLButtonUp(nFlags, point);
  Invalidate();

  UINT id = GetDlgCtrlID();
  if (id == IDC_CHECKPOSTBLIND)
  {
    static_cast<CActionBar*>(GetParent())->
      onCheckPostBlind();
  }
  else if (id == IDC_CHECKSITOUT)
  {
    static_cast<CActionBar*>(GetParent())->
      onCheckSitout();
  }
  else if (id == IDC_CHECKCALL)
  {
    static_cast<CActionBar*>(GetParent())->
      onCheckCallInTurn();
  }
  else if (id == IDC_CHECKRAISE)
  {
    static_cast<CActionBar*>(GetParent())->
      onCheckRaiseInTurn();
  }
  else if (id == IDC_CHECKFOLD)
  {
    static_cast<CActionBar*>(GetParent())->
      onCheckFoldInTurn();
  }
  else if (id == IDC_MUCKHAND)
  {
    static_cast<CActionBar*>(GetParent())->
      onCheckMuckHand();
  }
}
