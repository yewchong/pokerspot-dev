/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class House
  Created  : 25.10.1999

  OVERVIEW : Implements House player for Texas Hold'em.

 ****************************************************************************/

#include "stdafx.h"
#include "tem/house.h"
#include "base/cards.h"
#include "base/draw.h"

namespace
{
  LPCTSTR gszHouse = _T("");
}


BOOL LoadDeckImage(LPCTSTR pszImage, LPCTSTR pszMask,
                   CBitmap* pImage, CBitmap* pMask,
                   CBitmap* pBuf, int& width, int& height,
                   CString* pErrMsg)
{
  CBitmap image, mask;

  if (!image.Attach((HBITMAP)LoadImage(NULL, pszImage,
                                       IMAGE_BITMAP,
                                       0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load deck image file: %s", pszImage);
    return FALSE;
  }

  if (!mask.Attach((HBITMAP)LoadImage(NULL, pszMask,
                                      IMAGE_BITMAP,
                                      0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load deck mask file: %s", pszMask);
    return FALSE;
  }

  BITMAP bm;
  image.GetBitmap(&bm);
  width = bm.bmWidth;
  height = bm.bmHeight;

  // Now save bitmaps 
  pImage->DeleteObject();
  pImage->Attach(image.Detach());
  pMask->DeleteObject();
  pMask->Attach(mask.Detach());

  // Make buffer of correct size & format
  pBuf->DeleteObject();
  pBuf->Attach(::CopyImage(pImage->GetSafeHandle(),
                           IMAGE_BITMAP, bm.bmWidth, bm.bmHeight, 0));  

  return TRUE;
}


CBitmap House::BmpBuf_;

House::House(PlayerSlot& slot)
  :
  Base::Player(slot, gszHouse, 0),
  width_(0),
  height_(0)
{
  LoadDeckImage("res\\deck.bmp", "res\\deckmask.bmp",
                &deckImage_, &deckMask_, &BmpBuf_,
                width_, height_, NULL);
}


BOOL TransBlt(CDC* pDC, int xpos, int ypos,
              int width, int height,
              CBitmap* pImage,
              CBitmap* pMask,
              CBitmap* pBuf)
{
  CDC bufDC, memDC;

  BOOL rc = bufDC.CreateCompatibleDC(pDC) &&
            memDC.CreateCompatibleDC(pDC);

  if (rc)
  {
    // Copy current background to buf
    CBitmap* pOldBmpBuf = bufDC.SelectObject(pBuf);
    bufDC.BitBlt(0, 0, width, height,
                 pDC, xpos, ypos, SRCCOPY);

    // Transparent blit in to buf

    CBitmap* pOldBmpMem = memDC.SelectObject(pMask);    

    bufDC.BitBlt(0, 0, width, height,
                 &memDC, 0, 0, SRCAND);
    memDC.SelectObject(pImage);
    bufDC.BitBlt(0, 0, width, height,
                 &memDC, 0, 0, SRCINVERT);

    // Now blt the buffer to target dc
    pDC->BitBlt(xpos, ypos, width, height,
                &bufDC, 0, 0, SRCCOPY);

    // Clean up
    bufDC.SelectObject(pOldBmpBuf);
    memDC.SelectObject(pOldBmpMem);
  }
  return rc;
}

#define ncThis (const_cast<House*>(this))

BOOL House::drawImage(CDC* pDC, const CPoint& origo, DWORD dwFlags) const
{
  BOOL rc = TRUE;

  CSize size = getSize();
  CRect area(origo, size); 

  if (drawBounds())
    Base::DrawBumpRect(pDC, area, colorLight(), colorDark());

  if (deckImage_.GetSafeHandle())
  {
    TransBlt(pDC, origo.x, origo.y,
             width_, height_,
             &(ncThis->deckImage_),
             &(ncThis->deckMask_), 
             &(ncThis->BmpBuf_));
  }
  else
  {
    if (slot_.backsidePos_ != CPoint(-1, -1))
    {
      // Draw backside of at most NumDrawCards cards
      int c = 0;
      int i = min(cards_.size(), NumDrawCards); 
      if (i > 0)
      {
        Base::Card card = cards_.begin()->card_;
        for (; i > 0; --i, c++)
        {    
          CPoint pos = getCardPos(origo, c);
          rc = Base::Cards::Instance().drawCard(pDC, card, pos.x, pos.y,
                                                Base::Cards::C_BackSide);
        }
      }
    }
  }

  return rc;
}


void House::mouseClick(const CPoint&)
{ // Override to do nothing
}


//
// Return position of 'index'th card
// in local coordinates.
//
CPoint House::getCardPos(const CPoint& origo, int index) const
{ 
  CSize size = getSize();

  int c = index;
  if (c == LastCard)
    c = cards_.size();
  else
  { // drawing-wise, we're only interested
    // at most NumDrawCards cards 
    c = min(c, NumDrawCards); 
  }

  if (c > (cards_.size() - 1))
    c = (cards_.size() > 0) ? (cards_.size() - 1) : 0;

  return CPoint(origo.x + c * 2,
                -CardYOffset + origo.y + size.cy - Base::Cards::Height_ + c);
}


//
// Return position of 'index'th card
// in world coordinates.
//
CPoint House::getCardPos(int index) const
{ 
  CSize size = getSize();

  int c = index;
  if (c == LastCard)
    c = cards_.size();
  else
  { // drawing-wise, we're only interested
    // at most NumDrawCards cards 
    c = min(c, NumDrawCards); 
  }

  if (c > (cards_.size() - 1))
    c = (cards_.size() > 0) ? (cards_.size() - 1) : 0;

  return CPoint(slot_.pos_.x + c * 2,
    -CardYOffset + slot_.pos_.y + size.cy - Base::Cards::Height_ + c);
}


CSize House::getSize() const
{
  return CSize(Base::Cards::Width_ + 10,
               Base::Cards::Height_ + 10);
}