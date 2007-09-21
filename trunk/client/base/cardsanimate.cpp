/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Base::CardsAnimate
  Created  : 04/27/2000

  OVERVIEW : A class for doing cards animation.

 ****************************************************************************/

#include "stdafx.h"
#include "base/cardsanimate.h"
#include "base/cards.h"
#include "base/player.h"
#include "ui/tableview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace Base
{

CardsAnimate::CardsAnimate()
  :
  width_(0), height_(0),
  ticks_(0),
  msecs_(0),
  start_(0, 0),
  end_  (0, 0),
  pos_  (0, 0),
  pView_(NULL)
{
  pView_ = CTableView::Instance();
  ASSERT(pView_);
}


void CardsAnimate::startAnimate(int fromIndex,
                                int toIndex,
                                int numCards,
                                UINT msecs)
{
  ASSERT(pView_);
  if (!pView_) return;

  if (pos_.x != 0 || pos_.y != 0)
  { // clear previous animation
    stopAnimate();
  }

  Player* pFrom = pView_->getPlayer(fromIndex);
  Player* pTo = pView_->getPlayer(toIndex);
  if (pFrom && pTo)
  {
    // Prepare the animation bitmaps!
    if (!loadBitmaps(numCards))
      return;

    msecs_ = msecs;
    ticks_ = 0;
    start_ = pFrom->getClosedCardPos(0);
    end_ = pTo->getCardPos();
    pos_ = start_;

    CDC* pDC = pView_->GetDC();
    if (pDC)
    {
      Cards::Instance().startAnimate(pDC,
                                     bmpImage_,
                                     bmpMask_,
                                     width_,
                                     height_,
                                     start_);
      pView_->ReleaseDC(pDC);
    }
  }
}


void CardsAnimate::stopAnimate()
{
  ASSERT(pView_);
  if (!pView_) return;

  bmpImage_.DeleteObject();
  bmpMask_.DeleteObject();
  ticks_ = 0;
  msecs_ = 0;

  CDC* pDC = pView_->GetDC();
  if (pDC)
  {
    Cards::Instance().stopAnimate(pDC, width_, height_, pos_);
    pView_->ReleaseDC(pDC);
  }

  // Make sure all garbage is cleared from screen
  pView_->InvalidateRect(CRect(pos_, CSize(width_, height_)));
  width_ = height_ = 0;
}


BOOL CardsAnimate::stepAnimate(DWORD dt)
{
  ASSERT(pView_);
  if (!pView_) return FALSE;

  if (msecs_ == 0)
    return FALSE;

  BOOL rc = TRUE;

  ticks_ += dt;

  if (ticks_ < msecs_)
  { //
    // Do a piece of the animation
    //
    CPoint diff = end_ - start_;
    float d = float(ticks_) / float(msecs_);
    diff.x *= d;
    diff.y *= d;
    CPoint newPos = start_ + diff;

    animate(newPos, pos_);
 
    pos_ = newPos;
  }
  else 
  { // 
    // Animation is complete
    //
    stopAnimate();
    rc = FALSE;
  }

  return rc;
}


void CardsAnimate::animate(const CPoint& newPos,
                           const CPoint& oldPos)
{
  ASSERT(pView_);
  if (!pView_) return;

  CDC* pDC = pView_->GetDC();
  if (pDC)
  {
    Cards::Instance().animate(pDC,
                              bmpImage_,
                              bmpMask_,
                              width_, height_,
                              newPos, oldPos);
    pView_->ReleaseDC(pDC);
  }
}


bool CardsAnimate::loadBitmaps(int numCards)
{
  if (bmpImage_.GetSafeHandle())
    bmpImage_.DeleteObject();

  if (bmpMask_.GetSafeHandle())
    bmpMask_.DeleteObject();

  CString s;
  s.Format("back%i_image.bmp", numCards);
  if (!bmpImage_.Attach((HBITMAP)::LoadImage(NULL, s,
                                             IMAGE_BITMAP,
                                             0, 0,
                                             LR_LOADFROMFILE)))
  { // bitmap for 'numCards' not available,
    // use default which is 2
    numCards = 2;
    s = "back2_image.bmp";

    if (!bmpImage_.Attach((HBITMAP)::LoadImage(NULL, s,
                                               IMAGE_BITMAP,
                                               0, 0,
                                               LR_LOADFROMFILE)))
      return false;
  }

  s.Format("back%i_mask.bmp", numCards);
  if (!bmpMask_.Attach((HBITMAP)::LoadImage(NULL, s,
                                            IMAGE_BITMAP,
                                            0, 0,
                                            LR_LOADFROMFILE)))
    return false;

  BITMAP bm;
  memset(&bm, 0, sizeof(bm)); 
  bmpImage_.GetBitmap(&bm);
  width_ = bm.bmWidth;
  height_ = bm.bmHeight;

  return true;
}

} // Base

