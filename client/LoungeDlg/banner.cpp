#include "stdafx.h"
#include "loungedlg/banner.h"
#include "loungedlg/resource.h"


namespace
{
  CBanner* g_pBanner = NULL;
}

CBanner::CBanner(CWnd* wnd, const CRect& r)
    :
    wnd_  (wnd),
    pos_  (r.right, r.top + 2),
    rect_ (r),
    timer_(0)
{
  g_pBanner = this;
}


CBanner::~CBanner()
{
  g_pBanner = NULL;
}


void CBanner::setBannerText(const CString& s)
{
    text_ = s;
}


void CBanner::setBannerTextScroll(const CString& s, int speed)
{
    text_ = s;
    // Set up the timer
}


void CBanner::onDraw(CDC* pDC)
{
    // First blt the background
    CDC memDC;
    memDC.CreateCompatibleDC(pDC);
    CBitmap* pOldBmp = memDC.SelectObject(&bmp_);
    pDC->BitBlt(rect_.left, rect_.top, rect_.Width(), rect_.Height(),
                &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);

    // Draw the 3d rect
    pDC->Draw3dRect(&rect_, RGB(10, 10, 10), RGB(250, 250, 250));

    // Set clipping rect
    CRgn rgn;
    rgn.CreateRectRgnIndirect(&rect_);
    pDC->SelectClipRgn(&rgn);

    // Finally draw the text
    int oldMode = pDC->SetBkMode(TRANSPARENT);
    int oldColor = pDC->SetTextColor(RGB(255, 255, 0));

    CRect r1(pos_, rect_.BottomRight());
    CRect r2(r1);
    pDC->DrawText(text_, &r2, DT_LEFT|DT_CALCRECT);
    r2.IntersectRect(&r2, &rect_);
    pDC->DrawText(text_, &r1, DT_LEFT);

    pDC->SetBkMode(oldMode);

    if (r2.IsRectEmpty())
    {
      pos_ = CPoint(rect_.right, rect_.top + 2);
    }
}


void CBanner::tick(DWORD)
{
    pos_.x -= 10;
    CDC* pDC = wnd_->GetDC();
    if (pDC)
    {
      onDraw(pDC);
      wnd_->ReleaseDC(pDC);
    }    
}


void CALLBACK CBanner::BannerProc(HWND, UINT, UINT, DWORD msecs)
{
  if (g_pBanner)
    g_pBanner->tick(msecs);
}

void CBanner::start()
{
  if (timer_ != 0)
  {
    ::KillTimer(NULL, timer_);
  }
  timer_ = SetTimer(NULL, ID_TIMER_BANNER, 500, BannerProc);

  if (timer_ != 0)
  {
     // Create a black background bitmap
    CDC* pDC = wnd_->GetDC();
    if (pDC)
    {
      CDC dcMem;
      dcMem.CreateCompatibleDC(pDC);
      bmp_.CreateCompatibleBitmap(pDC, rect_.Width(), rect_.Height());
      CBitmap* pOldBmp = dcMem.SelectObject(&bmp_);

      dcMem.BitBlt(0, 0, rect_.Width(), rect_.Height(),
                   NULL, 0, 0, BLACKNESS);
      dcMem.SelectObject(pOldBmp);

      wnd_->ReleaseDC(pDC);
    }    
  }
}

void CBanner::stop()
{
  if (timer_ != 0)
  {
    ::KillTimer(NULL, timer_);
    timer_ = NULL;
  }
}