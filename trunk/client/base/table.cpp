/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Table
  Created  : 29.10.1999

  OVERVIEW : Class table represents one table image in
             the floor map.

 ****************************************************************************/


#include "stdafx.h"
#include "base/table.h"
#include "base/draw.h"
#ifndef LOUNGE_DLG_
  #include "poker.h"
#endif

namespace Base
{

CSize  Table::Size_ = CSize(100, 100);
CBrush Table::Brush_;
CDib   Table::DibImage_;

Table::Table(USHORT tableNumber, 
             const CPoint& pos, LPCTSTR name,
             int players, int maxPlayers,
             USHORT lolimit, USHORT hilimit)
  :
  number_    (tableNumber),
  pos_       (pos),
  name_      (name),
  players_   (players),
  maxPlayers_(maxPlayers),
  lolimit_   (lolimit),
  hilimit_   (hilimit)
{
  if (!Brush_.GetSafeHandle())
    Brush_.CreateSolidBrush(RGB(0, 0, 196));

#ifndef LOUNGE_DLG_
  static bool loaded = false;
  if (!loaded)
  {
    loaded = true;
    CBitmap tmp;
    if (tmp.Attach(::LoadImage(NULL, "table.bmp", IMAGE_BITMAP, 0, 0,
		    LR_LOADFROMFILE | LR_CREATEDIBSECTION)) ||
        tmp.LoadBitmap(IDB_TABLE))
    {
      BITMAP bm;
      tmp.GetBitmap(&bm);
      if (DibImage_.Create(bm.bmWidth, bm.bmHeight))
      {
        Size_.cx = DibImage_.W;
        Size_.cy = DibImage_.H;

        CDC dc;
        dc.CreateCompatibleDC(NULL);
        CBitmap* pOldBmp = dc.SelectObject(&tmp);
        DibImage_.CopyFromDCRect(&dc, 0, 0, Size_.cx, Size_.cy);
        tmp.Detach();
      } 
    }
  }
#endif
}


Table::~Table()
{
}



BOOL Table::draw(CDC* pDC, DWORD flags) const
{
#ifndef LOUNGE_DLG_
  CRect r(pos_, Size_);

  if (DibImage_.m_Bitmap != NULL)
  {
    static CDib buf;
    if (buf.m_Bitmap == NULL)
      buf.Create(Size_.cx, Size_.cy);
    buf.CopyFromDCRect(pDC, pos_.x, pos_.y, Size_.cx, Size_.cy);
    buf.BlendDib(&DibImage_, 80);
    buf.CopyToDCRect(pDC, pos_.x, pos_.y);
  }
  else
  {
    pDC->FillRect(r, &Brush_);
  }

  Base::DrawBumpRect(pDC, r, RGB(0, 0, 0), RGB(255, 255, 255));

  // Display table info
  pDC->SetBkMode(TRANSPARENT);
  CFont* pOldFont = pDC->SelectObject(CPokerApp::GetBoldFont());

  CString s;
  s.Format(_T("#%i\n%s\n%i/%i\n$%i-%i"),
           number_, name_, players_, maxPlayers_,
           lolimit_, hilimit_);

  // Center both vertically and horizontally
  CRect r2(r);
  Base::DrawBumpText(pDC, (LPCSTR)s,
                     r2, DT_CENTER | DT_CALCRECT,
                     RGB(255, 255, 0), RGB(0, 0, 0));

  r.OffsetRect(0, r.Height() / 2 - r2.Height() / 2);
  Base::DrawBumpText(pDC, (LPCSTR)s,
                     r, DT_CENTER | DT_NOCLIP,
                     RGB(255, 255, 0), RGB(0, 0, 0));  
  
  pDC->SelectObject(pOldFont);
#endif
  return TRUE;
}


} // Base

