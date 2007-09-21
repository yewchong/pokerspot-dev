/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Cards
  Created  : 25.10.1999

  OVERVIEW : Class for drawing & animating cards.

 ****************************************************************************/

#include "stdafx.h"
#include "base/cards.h"
#include "base/card.h"
#include "base/misc.h"
#include "ui/resource.h"
#include "ui/tableview.h"
#include <vector>
#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
  // Module globals
  Base::Cards g_Cards;

  // Flip animation
  const int NumFlipX = 8;
  const int NumFlipY = 16;
  const int FlipSleepTime = 1;
  typedef std::vector<CRect> Rects;
  Rects g_Rects;
}


namespace Base
{

//
// This mfunction does one animation step.
//
BOOL Cards::DrawSprite(HDC hdc, 
                       const CPoint& newPos,
                       const CPoint& oldPos,
                       int width, int height,
                       HBITMAP mHSave,
                       HBITMAP mHImage,
                       HBITMAP mHMask)
{
  HBITMAP hbmHold = 0;
  HDC hdcMem = 0,
      hdcMemHold = 0;
  RECT rectNew,
       rectOld,
       rectUnion;
    
  // copy entire affected area of window to working bitmap:
  rectOld.left = oldPos.x;
  rectOld.top = oldPos.y;
  rectOld.right = oldPos.x + width;
  rectOld.bottom = oldPos.y + height;

  rectNew.left = newPos.x;
  rectNew.top = newPos.y;
  rectNew.right = newPos.x + width;
  rectNew.bottom = newPos.y + height;
  
  ::UnionRect(&rectUnion, &rectOld, &rectNew);
  rectUnion.left -= rectUnion.left % 8;
  
  hbmHold = ::CreateCompatibleBitmap(hdc,
                rectUnion.right - rectUnion.left,
                rectUnion.bottom - rectUnion.top);
  if (hbmHold == NULL)
    return FALSE;
  
  hdcMemHold = CreateCompatibleDC(hdc);
  HBITMAP hbmMemHoldOld = (HBITMAP)SelectObject(hdcMemHold, hbmHold);
  
  // save it
  ::BitBlt(hdcMemHold, 0, 0, 
           rectUnion.right - rectUnion.left,
           rectUnion.bottom - rectUnion.top,
           hdc, rectUnion.left, rectUnion.top, SRCCOPY);
  
  // create another memory device context:
  hdcMem = ::CreateCompatibleDC(hdc);
  
  // restore window graphics at prior position if it was saved:
  HBITMAP hbmMemOld = (HBITMAP)::SelectObject(hdcMem, mHSave);
  if (oldPos != CPoint(-1, -1))
  {

    ::BitBlt(hdcMemHold,
             oldPos.x - rectUnion.left,
             oldPos.y - rectUnion.top,
             width, height,
             hdcMem, 0, 0, SRCCOPY);
  }
  
  // save current window graphics:
  ::BitBlt(hdcMem,
           0, 0, width, height,
           hdcMemHold,
           newPos.x - rectUnion.left,
           newPos.y - rectUnion.top, SRCCOPY);
  
  // display image bitmap first time:
  ::SelectObject(hdcMem, mHImage);
  ::BitBlt(hdcMemHold,
           newPos.x - rectUnion.left,
           newPos.y - rectUnion.top,
           width, height,
           hdcMem, 0, 0, SRCINVERT);
  
  // display mask bitmap:
  ::SelectObject(hdcMem, mHMask);
  ::BitBlt(hdcMemHold,
           newPos.x - rectUnion.left,
           newPos.y - rectUnion.top,
           width, height,
           hdcMem, 0, 0, SRCAND);
  
  // display image bitmap second time:
  ::SelectObject(hdcMem, mHImage);
  ::BitBlt(hdcMemHold,
           newPos.x - rectUnion.left,
           newPos.y - rectUnion.top,
           width, height,
           hdcMem, 0, 0, SRCINVERT);
  
  // copy working bitmap back to window:
  ::BitBlt(hdc, rectUnion.left, rectUnion.top,
           rectUnion.right - rectUnion.left,
           rectUnion.bottom - rectUnion.top,
           hdcMemHold, 0, 0, SRCCOPY);
  
  // Clean up & delete the memory device contexts:
  ::SelectObject(hdcMemHold, hbmMemHoldOld);
  ::SelectObject(hdcMem, hbmMemOld);

  ::DeleteDC(hdcMemHold);
  ::DeleteDC(hdcMem);
  
  // delete working bitmap:
  ::DeleteObject(hbmHold);

  return TRUE; 
}


int Cards::Width_ = 71,
    Cards::Height_ = 96,
    Cards::XOffset_ = 14,
    Cards::YOffset_ = 0;

Cards::Cards()
{}


Cards::~Cards()
{}


Cards& Cards::Instance()
{
  return g_Cards;
}


//
// This version loads the cards from 
// resource.
//
BOOL Cards::loadBitmaps()
{
  bmpCards_.DeleteObject();
  bmpCardsMask_.DeleteObject();
  bmpBack_.DeleteObject();
  bmpBackMask_.DeleteObject();

  BOOL rc = bmpCards_.LoadBitmap(IDB_CARDS);
  ASSERT(rc);
  if (rc)
  {
    BITMAP bm;
    bmpCards_.GetBitmap(&bm);
    Cards::Height_ = bm.bmHeight / 4;
    Cards::Width_ = bm.bmWidth / 13;
  }

  OpenCardWidth_ = Cards::Width_;
  OpenCardHeight_ = Cards::Height_;
  BacksideCardWidth_ = Cards::Width_;
  BacksideCardHeight_ = Cards::Height_;

  bmpCardsMask_.LoadBitmap(IDB_MASK);
  bmpBack_.LoadBitmap(IDB_BACK);
  bmpBackMask_.LoadBitmap(IDB_MASK);

  return rc;
}


BOOL Cards::drawCard(CDC* pDC, const Card& card, int xpos, int ypos, DWORD dwFlags)
{
  // Card number must be between [2, 14] and suit between [1,4]
  //ASSERT((dwFlags & C_BackSide) || card.number() >= 2 && card.number() <= 14);
  //ASSERT((dwFlags & C_BackSide) || card.suit() > SuitBegin && card.suit() < SuitEnd);

  // If the value is not between [2,14] and suit is not between [1,4],
  // then render the card face down
  bool drawFaceDown = 
         (!((dwFlags & C_BackSide) || card.number() >= 2 && card.number() <= 14) ||
          !((card.suit() >= SuitBegin && card.suit() < SuitEnd)));

  BOOL rc = TRUE;

  int cardx = 0;
  int cardy = 0;

  if (dwFlags & C_BackSide)
  {
    rc = bltCard(pDC, xpos, ypos, 
                 BacksideCardWidth_, BacksideCardHeight_,
                 0, 0, &bmpBack_, &bmpBackMask_);
  }
  else if (drawFaceDown)
  { 
    CBitmap& cardImage = getImage(card);
    if (cardImage.GetSafeHandle())
    {
      rc = bltCard(pDC, xpos, ypos, Width_, Height_,
                  cardx, cardy, &cardImage, &bmpCardsMask_);
    }
    else
    { // Draw special invalid card to make it
      // obvious there's a problem
      bmpInvalid_.LoadBitmap(IDB_INVALIDCARD);
      rc = bltCard(pDC, xpos, ypos, Width_, Height_,
                   0, 0, &bmpInvalid_, &bmpCardsMask_);
      bmpInvalid_.DeleteObject();
    }
  }
  else
  {
    CBitmap& cardImage = getImage(card);
    rc = bltCard(pDC, xpos, ypos, Width_, Height_,
                 cardx, cardy, &cardImage, &bmpCardsMask_);
  }

  return rc;
}


BOOL Cards::bltCard(CDC* pDC,
                    int xpos, int ypos,
                    int width, int height,
                    int cardx, int cardy,
                    CBitmap* pBmpImage,
                    CBitmap* pBmpMask)
{
  CDC bufDC, memDC, transDC;

  BOOL rc = bufDC.CreateCompatibleDC(pDC) &&
            memDC.CreateCompatibleDC(pDC) &&
            transDC.CreateCompatibleDC(pDC);

  if (rc)
  {
    BITMAP bm;
    rc = bmpCardsMask_.GetBitmap(&bm);
    if (rc)
      rc = bmpBuf_.CreateBitmapIndirect(&bm);
  }

  if (rc)
  {
    // Copy current background to buf
    CBitmap* pOldBmpBuf = bufDC.SelectObject(&bmpBuf_);
    bufDC.BitBlt(0, 0, width, height,
                 pDC, xpos, ypos, SRCCOPY);

    // Transparent blit in to buf

    CBitmap* pOldBmpMem = memDC.SelectObject(pBmpImage);
    CBitmap* pOldBmpTrans = transDC.SelectObject(pBmpMask);

    bufDC.BitBlt(0, 0, width, height,
                 &transDC, 0, 0, SRCAND);
    bufDC.BitBlt(0, 0, width, height,
                 &memDC, cardx, cardy, SRCINVERT);

    // Now blt the buffer to target dc
    pDC->BitBlt(xpos, ypos, width, height,
                &bufDC, 0, 0, SRCCOPY);

    // Clean up
    bufDC.SelectObject(pOldBmpBuf);
    memDC.SelectObject(pOldBmpMem);
    transDC.SelectObject(pOldBmpTrans);

    bmpBuf_.DeleteObject();
  }

  return rc;
}


BOOL Cards::animate(CDC* pDC, const Card& card, const CPoint& newPos, const CPoint& oldPos, DWORD dwFlags)
{
  int width = (dwFlags & C_BackSide) ? BacksideCardWidth_ : Width_;
  int height = (dwFlags & C_BackSide) ? BacksideCardHeight_ : Height_;

  HDC hdc = pDC->GetSafeHdc();
  HBITMAP hbmSave = (HBITMAP)bmpSave_.GetSafeHandle();
  ASSERT(hbmSave != NULL);
  HBITMAP hbmImage = NULL;
  HBITMAP hbmMask = NULL;

  if (dwFlags & C_BackSide)
  {
    hbmImage = (HBITMAP)bmpBack_.GetSafeHandle();
    hbmMask = (HBITMAP)bmpBackMask_.GetSafeHandle();
  }
  else
  {
    // Open card animation NOT IMPLEMENTED
  }

  return DrawSprite(hdc, newPos, oldPos,
                    width, height,
                    hbmSave, hbmImage, hbmMask);
}


BOOL Cards::startAnimate(CDC* pDC, const CPoint& pos, DWORD dwFlags)
{ 
  // Create buffer for animation
  if (!bmpSave_.GetSafeHandle())
  {
    BITMAP bm;
    if (!bmpBack_.GetBitmap(&bm))
      return FALSE;
    if (!bmpSave_.CreateBitmapIndirect(&bm))
      return FALSE;
  }

  int width = (dwFlags & C_BackSide) ? BacksideCardWidth_ : Width_;
  int height = (dwFlags & C_BackSide) ? BacksideCardHeight_ : Height_;

  // Save area at 'pos'
  CDC memDC;
  BOOL rc = memDC.CreateCompatibleDC(pDC);
  if (rc)
  {
    CBitmap* pOldBmp = memDC.SelectObject(&bmpSave_);
    memDC.BitBlt(0, 0, width, height,
                 pDC, pos.x, pos.y, SRCCOPY);
    memDC.SelectObject(pOldBmp);
  }

  return rc;
}


BOOL Cards::stopAnimate(CDC* pDC, const CPoint& pos, DWORD dwFlags)
{
  int width = (dwFlags & C_BackSide) ? BacksideCardWidth_ : Width_;
  int height = (dwFlags & C_BackSide) ? BacksideCardHeight_ : Height_;

  // Restore area at 'pos'
  CDC memDC;
  BOOL rc = memDC.CreateCompatibleDC(pDC);
  if (rc)
  {
    CBitmap* pOldBmp = memDC.SelectObject(&bmpSave_);
    pDC->BitBlt(pos.x, pos.y, width, height,
                &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
  }

  // Destroy animation buffer
  bmpSave_.DeleteObject();

  return rc;
}



//
// Start a "flipping" sequence. A flip sequence
// is the animation when a card is shown with a
// special animation. This mfunction returns the
// number of steps in the sequence.
//
// Generate a number of small rectangles that
// split the card area into sectors and randomize
// the order of the rectangles. The 'flip' mfunction
// will then iterate over the rectangles and blit
// parts of the card image as it goes.
//
int Cards::startFlip(const Card& card, const CPoint& pos)
{
  static bool initted = false;
  if (!initted)
  {
    initted = true;

    // XXX HARDCODE: the division is not even =>
    // this works with height 96
    int height = Height_ / NumFlipY + 1;
    int width = Width_ / NumFlipX;

    // Initial y position is negative since it'll
    // be offset before put to the store.
    CRect r(0, -height, width, 0);

    // Since the size/numflips division is not likely to be even, the last 
    // vertical and horizontal strips need to be a bit larger
    // to cover the whole card.
    int extraY = Height_ - (height * NumFlipY);
    int extraX = Width_ - (width * NumFlipX);  

    for (int i = 0; i < NumFlipX; i++)
    {
      if (i == (NumFlipX - 1))
        r.right += extraX;

      for (int j = 0; j < NumFlipY; j++)
      {
        if (j == (NumFlipY - 1))
          r.bottom += extraY;

        r.OffsetRect(0, height);
        g_Rects.push_back(r);
      }

      r.top = -height;
      r.bottom = 0;
      r.OffsetRect(width, 0);
    }
  }

  // Randomize the sequence
  std::random_shuffle(g_Rects.begin(), g_Rects.end());
  return g_Rects.size();
}


//
// Blit the card image in a number of smaller
// random blits.
//
// This mfunction does the whole animation at
// one call and it lasts a while.
//
BOOL Cards::flip(CDC* pDC, const Card& card,
                 const CPoint& pos, DWORD dwFlags)
{
  const int SleepSteps = 3;
  int sleep = SleepSteps;

  CBitmap bmpTmp;
  bmpTmp.CreateCompatibleBitmap(pDC, Width_, Height_);

  CDC memDC;
  BOOL rc = memDC.CreateCompatibleDC(pDC);

  if (rc)
  {
    // First draw image in temporary buffer
    CBitmap* pOldBmp = memDC.SelectObject(&bmpTmp);
    memDC.BitBlt(0, 0, Width_, Height_,
                 pDC, pos.x, pos.y, SRCCOPY);
    drawCard(&memDC, card, 0, 0, 0);

    // Then blit one rect at a time from temporary
    // buffer to target DC.
    for (Rects::const_iterator i = g_Rects.begin(),
                               e = g_Rects.end();
         i != e;
         ++i)
    {
      CRect r = *i;

      pDC->BitBlt(pos.x + r.left, pos.y + r.top,
                  r.Width(), r.Height(),
                  &memDC, r.left, r.top, SRCCOPY);
      // Sleep on every second iteration
      sleep--;
      if (sleep == 0)
      {
        Sleep(FlipSleepTime);
        sleep = SleepSteps;
      }      
    }

    memDC.SelectObject(pOldBmp);
  }

  return rc;
}


int Cards::OpenCardWidth_ = 0, Cards::OpenCardHeight_ = 0,
    Cards::BacksideCardWidth_ = 0, Cards::BacksideCardHeight_ = 0;


//
// Load the "open card" image & mask bitmap files and
// attach them to the bitmap data members.
//
// This is the old one-large-bitmap version.
//
BOOL Cards::loadOpenCardBitmaps2(LPCTSTR pszImage,
                                 LPCTSTR pszMask,
                                 CString* pErrMsg)
{
  CBitmap image, mask;

  if (!image.Attach((HBITMAP)LoadImage(NULL, pszImage,
                                       IMAGE_BITMAP,
                                       0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load card image file: %s", pszImage);
    return FALSE;
  }

  if (!mask.Attach((HBITMAP)LoadImage(NULL, pszMask,
                                      IMAGE_BITMAP,
                                      0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load card mask file: %s", pszMask);
    return FALSE;
  }

  BITMAP bm;
  if (image.GetBitmap(&bm))
  {
    Width_ = bm.bmWidth / 13;
    Height_ = bm.bmHeight / 4;
    OpenCardWidth_ = bm.bmWidth / 13;
    OpenCardHeight_ = bm.bmHeight / 4;
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card image bitmap size: %s", pszImage);
    return FALSE;
  }

  if (mask.GetBitmap(&bm))
  { // Mask size must match one card
    if (OpenCardWidth_ != bm.bmWidth ||
        OpenCardHeight_ != bm.bmHeight)
    {
      if (pErrMsg)
        pErrMsg->Format("Card image and mask size do not match:"
                        " image: (%i, %i), mask: (%i, %i)",
                        OpenCardWidth_, OpenCardHeight_,
                        bm.bmWidth, bm.bmHeight);
      return FALSE;
    }
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card mask bitmap size: %s", pszMask);
    return FALSE;
  }

  // Now save bitmaps to data members
  bmpCards_.DeleteObject();
  bmpCards_.Attach(image.Detach());
  bmpCardsMask_.DeleteObject();
  bmpCardsMask_.Attach(mask.Detach());

  return TRUE;
}

//
// This function load the card images from
// 52 separate bitmaps.
//
BOOL Cards::loadOpenCardBitmaps(LPCTSTR pszImage,
                                LPCTSTR pszMask,
                                CString* pErrMsg)
{
#ifdef PRELOAD_CARDS_
  // Load all card graphics to bitmaps -
  // this is a HUGE resource hog!
  CBitmap image, mask;
  CString path = pszImage + CString("\\");
  const CString ext(".bmp");

  int i = 0;
  for (int suit = 1; suit < 5; suit++)
  {
    for (int val = 2; val < 15; val++)
    {
      CString name = path + Base::AsString(Card(suit, val));
      name += ext;
    
      bmpCard_[i].DeleteObject();

      HBITMAP hBmp = (HBITMAP)LoadImage(NULL, name,
                                       IMAGE_BITMAP,
                                       0, 0, LR_LOADFROMFILE);

      if (!hBmp || !bmpCard_[i++].Attach(hBmp))
      {
        if (pErrMsg)
          pErrMsg->Format("Can't load card image file: %s", name);
        return FALSE;
      }
    }
  }

  if (!mask.Attach((HBITMAP)LoadImage(NULL, pszMask,
                                      IMAGE_BITMAP,
                                      0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load card mask file: %s", pszMask);
    return FALSE;
  }

  BITMAP bm;
  if (bmpCard_[0].GetBitmap(&bm))
  {
    Width_ = bm.bmWidth;
    Height_ = bm.bmHeight;
    OpenCardWidth_ = bm.bmWidth;
    OpenCardHeight_ = bm.bmHeight;
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card image bitmap size: %s", pszImage);
    return FALSE;
  }

  if (mask.GetBitmap(&bm))
  { // Mask size must match one card
    if (OpenCardWidth_ != bm.bmWidth ||
        OpenCardHeight_ != bm.bmHeight)
    {
      if (pErrMsg)
        pErrMsg->Format("Card image and mask size do not match:"
                        " image: (%i, %i), mask: (%i, %i)",
                        OpenCardWidth_, OpenCardHeight_,
                        bm.bmWidth, bm.bmHeight);
      return FALSE;
    }
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card mask bitmap size: %s", pszMask);
    return FALSE;
  }

  // Now save bitmaps to data members
  bmpCardsMask_.DeleteObject();
  bmpCardsMask_.Attach(mask.Detach());

#else 
  // Don't load cards now but on demand!

  CBitmap image, mask;
  CString path = pszImage + CString("\\");
  const CString ext(".bmp");

  int suit = 1;
  int val = 2;
  CString name = path + Base::AsString(Card(suit, val));
  name += ext;
    
  HBITMAP hBmp = (HBITMAP)LoadImage(NULL, name,
                                    IMAGE_BITMAP,
                                    0, 0, LR_LOADFROMFILE);
  if (!hBmp)
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load card image file: %s", name);
    return FALSE;
  }

  image.Attach(hBmp);

  if (!mask.Attach((HBITMAP)LoadImage(NULL, pszMask,
                                      IMAGE_BITMAP,
                                      0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load card mask file: %s", pszMask);
    return FALSE;
  }

  BITMAP bm;
  if (image.GetBitmap(&bm))
  {
    Width_ = bm.bmWidth;
    Height_ = bm.bmHeight;
    OpenCardWidth_ = bm.bmWidth;
    OpenCardHeight_ = bm.bmHeight;
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card image bitmap size: %s", pszImage);
    return FALSE;
  }

  if (mask.GetBitmap(&bm))
  { // Mask size must match one card
    if (OpenCardWidth_ != bm.bmWidth ||
        OpenCardHeight_ != bm.bmHeight)
    {
      if (pErrMsg)
        pErrMsg->Format("Card image and mask size do not match:"
                        " image: (%i, %i), mask: (%i, %i)",
                        OpenCardWidth_, OpenCardHeight_,
                        bm.bmWidth, bm.bmHeight);
      return FALSE;
    }
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card mask bitmap size: %s", pszMask);
    return FALSE;
  }

  // Now save bitmaps to data members
  bmpCardsMask_.DeleteObject();
  bmpCardsMask_.Attach(mask.Detach());

#endif

  return TRUE;
}


CBitmap& Cards::getImage(const Card& card)
{
#ifndef PRELOAD_CARDS_
  CBitmap& bmp = bmpCardImage_;
  bmp.DeleteObject();

  CString path = "graphics\\";
  const CString ext(".bmp");
  CString name = path + Base::AsString(card);
  name += ext;
    
  HBITMAP hBmp = (HBITMAP)LoadImage(NULL, name,
                                    IMAGE_BITMAP,
                                    0, 0, LR_LOADFROMFILE);

  if (hBmp)
    bmp.Attach(hBmp);

  return bmp;

#else

  ASSERT((card.suit() - 1) * 13 + (card.number() - 2) < 52);
  if ((card.suit() - 1) * 13 + (card.number() - 2) < 52)
    return bmpCard_[(card.suit() - 1) * 13 + (card.number() - 2)];
  else
    return bmpInvalid_;

#endif
}


BOOL Cards::loadBacksideBitmaps(LPCTSTR pszImage,
                                LPCTSTR pszMask,
                                CString* pErrMsg)
{
  CBitmap image, mask;

  if (!image.Attach((HBITMAP)LoadImage(NULL, pszImage,
                                       IMAGE_BITMAP,
                                       0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load card image file: %s", pszImage);
    return FALSE;
  }

  if (!mask.Attach((HBITMAP)LoadImage(NULL, pszMask,
                                      IMAGE_BITMAP,
                                      0, 0, LR_LOADFROMFILE)))
  {
    if (pErrMsg)
      pErrMsg->Format("Can't load card mask file: %s", pszMask);
    return FALSE;
  }

  BITMAP bm;
  if (image.GetBitmap(&bm))
  {
    BacksideCardWidth_ = bm.bmWidth;
    BacksideCardHeight_ = bm.bmHeight;
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card image bitmap size: %s", pszImage);
    return FALSE;
  }

  if (mask.GetBitmap(&bm))
  { // Mask size must match one card
    if (BacksideCardWidth_ != bm.bmWidth ||
        BacksideCardHeight_ != bm.bmHeight)
    {
      if (pErrMsg)
        pErrMsg->Format("Card image and mask size do not match:"
                        " image: (%i, %i), mask: (%i, %i)",
                        OpenCardWidth_, OpenCardHeight_,
                        bm.bmWidth, bm.bmHeight);
      return FALSE;
    }
  }
  else
  {
    if (pErrMsg)
      pErrMsg->Format("Can't get card mask bitmap size: %s", pszMask);
    return FALSE;
  }

  bmpBack_.DeleteObject();
  bmpBackMask_.DeleteObject();

  bmpBack_.Attach(image.Detach());
  bmpBackMask_.Attach(mask.Detach());

  return TRUE;
}


BOOL Cards::animate(CDC* pDC, const CBitmap& bmpImage, const CBitmap& bmpMask,
                    int width, int height, const CPoint& newPos, const CPoint& oldPos)
{
  HDC hdc = pDC->GetSafeHdc();
  HBITMAP hbmSave = (HBITMAP)bmpSave_.GetSafeHandle();
  ASSERT(hbmSave != NULL);

  return DrawSprite(hdc, newPos, oldPos,
                    width, height,
                    hbmSave, 
                    (HBITMAP)bmpImage.GetSafeHandle(),
                    (HBITMAP)bmpMask.GetSafeHandle());
}


BOOL Cards::startAnimate(CDC* pDC, const CBitmap& bmpImage, const CBitmap& bmpMask,
                         int width, int height, const CPoint& pos)
{
  // Create the animation buffer bitmap
  bmpSave_.DeleteObject();
  BITMAP bm;
  if (!const_cast<CBitmap&>(bmpImage).GetBitmap(&bm))
    return FALSE;
  if (!bmpSave_.CreateBitmapIndirect(&bm))
    return FALSE;

  // Save area at 'pos'
  CDC memDC;
  BOOL rc = memDC.CreateCompatibleDC(pDC);
  if (rc)
  {
    CBitmap* pOldBmp = memDC.SelectObject(&bmpSave_);
    memDC.BitBlt(0, 0, width, height,
                 pDC, pos.x, pos.y, SRCCOPY);
    memDC.SelectObject(pOldBmp);
  }

  return rc;
}


BOOL Cards::stopAnimate(CDC* pDC, int width, int height, const CPoint& pos)
{
  // Restore area at 'pos'
  CDC memDC;
  BOOL rc = memDC.CreateCompatibleDC(pDC);
  if (rc)
  {
    CBitmap* pOldBmp = memDC.SelectObject(&bmpSave_);
    pDC->BitBlt(pos.x, pos.y, width, height,
                &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
  }

  // Destroy animation buffer
  bmpSave_.DeleteObject();

  return rc;
}


} // Base

