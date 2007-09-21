/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Chips
  Created  : 03/03/2000

  OVERVIEW : Class for drawing & animating chips.

 ****************************************************************************/

#include "stdafx.h"
#include "base/graphic_chips.h"
#include "base/cards.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
  Base::Chips g_Chips;

  const int ChipYOffset = 4;
}


namespace Base
{

int Chips::Width_ = 22;
int Chips::Height_ = 17;

Chips& Chips::Instance()
{
  return g_Chips;
}


Chips::Chips()
  :
  chips_(0),
  drawChips_(false)
{}


Chips::~Chips()
{}


BOOL Chips::loadChipGraphics(LPCTSTR pszFileBase, CString* pErrMsg)
{
  BOOL rc = TRUE;

  rc = oneChip_.Attach((HBITMAP)LoadImage(NULL,
                                          "chip-1.bmp",
                                          IMAGE_BITMAP,
                                          0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 1 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = fiveChip_.Attach((HBITMAP)LoadImage(NULL,
                                          "chip-5.bmp",
                                          IMAGE_BITMAP,
                                          0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 5 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = tenChip_.Attach((HBITMAP)LoadImage(NULL,
                                          "chip-10.bmp",
                                          IMAGE_BITMAP,
                                          0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 10 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = twenty5Chip_.Attach((HBITMAP)LoadImage(NULL,
                                          "chip-25.bmp",
                                          IMAGE_BITMAP,
                                          0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 25 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = fiftyChip_.Attach((HBITMAP)LoadImage(NULL,
                                          "chip-50.bmp",
                                          IMAGE_BITMAP,
                                          0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 50 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = hundredChip_.Attach((HBITMAP)LoadImage(NULL,
                                          "chip-100.bmp",
                                          IMAGE_BITMAP,
                                          0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 100 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = fiveHundredChip_.Attach((HBITMAP)LoadImage(NULL,
                                                  "chip-500.bmp",
                                                  IMAGE_BITMAP,
                                                  0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 500 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = thousandChip_.Attach((HBITMAP)LoadImage(NULL,
                                               "chip-1000.bmp",
                                               IMAGE_BITMAP,
                                               0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 1000 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }

  rc = fiveThousandChip_.Attach((HBITMAP)LoadImage(NULL,
                                               "chip-5000.bmp",
                                               IMAGE_BITMAP,
                                               0, 0, LR_LOADFROMFILE));

  if (!rc)
  {
    AfxMessageBox("Unable to load 5000 chip image "
                  "- chip graphics will not be available.");
    return FALSE;
  }


  rc = chipMask_.Attach((HBITMAP)LoadImage(NULL,
                                          "chip-mask.bmp",
                                          IMAGE_BITMAP,
                                          0, 0, LR_LOADFROMFILE));

  chipMask2_.Attach((HBITMAP)LoadImage(NULL,
                                       "chip-mask2.bmp",
                                       IMAGE_BITMAP,
                                       0, 0, LR_LOADFROMFILE));


  if (rc)
  {
    BITMAP bm;
    if (chipMask_.GetBitmap(&bm))
    {
      Width_ = bm.bmWidth;
      Height_ = bm.bmHeight;
      if (Width_ > 0 && Height_ > 0)
      {
        CWindowDC dcScreen(NULL);
        rc = bmpBuf_.CreateCompatibleBitmap(&dcScreen, Width_, Height_);
      }
    }
  }

  drawChips_ = true;

  return rc;
}

#define ONE_STACK       5
#define TWO_STACKS      20
#define THREE_STACKS    30

BOOL Chips::drawChips(CDC* pDC, const CChips& cchips, int xpos, int ypos, DWORD dwFlags)
{
  UINT chips = cchips.getDollars();

  if (!drawChips_ || chips == 0)
    return TRUE;

  CBitmap* pMask = &chipMask_;

  int numChips = getNumChipUnits(chips);
  int numStacks = 1;
  int chipsPerStack = 1;

  if (numChips < ONE_STACK)
  {
    numStacks = 1;
    chipsPerStack = numChips;
  }
  else if (numChips < TWO_STACKS)
  {
    numStacks = 2;
    chipsPerStack = (numChips / 2) + 1;
  }
  else if (numChips < THREE_STACKS)
  {
    numStacks = 3;
    chipsPerStack = (numChips / 3) + 1;
  }
  else
  { // at most 4 stacks
    numStacks = 4;
    chipsPerStack = (numChips / 4) + 1;
  }

  CDC memDC, bufDC, transDC;

  BOOL rc = bufDC.CreateCompatibleDC(pDC) &&
            memDC.CreateCompatibleDC(pDC) &&
            transDC.CreateCompatibleDC(pDC);

  int next_x = xpos;
  int next_y = ypos;
  int cps = chipsPerStack;

  // First blt 5k chips
  if (chips >= 5000)
  {
    CBitmap* pImage = &fiveThousandChip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    int units = chips / 5000U;
    bltChips(pDC, units * 5000,
             5000, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
    chips -= (units * 5000);
  }

  // blt thousands
  if (chips >= 1000)
  {
    CBitmap* pImage = &thousandChip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    int units = chips / 1000U;
    bltChips(pDC, units * 1000,
             1000, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
    chips -= (units * 1000);
  }

  if (chips >= 500)
  {
    CBitmap* pImage = &fiveHundredChip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    int units = chips / 500U;
    bltChips(pDC, units * 500,
             500, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
    chips -= (units * 500);
  }

  if (chips >= 100)
  {
    CBitmap* pImage = &hundredChip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    int units = chips / 100U;
    bltChips(pDC, units * 100,
             100, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
    chips -= (units * 100);
  }

  if (chips >= 50)
  {
    CBitmap* pImage = &fiftyChip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    int units = chips / 50U;
    bltChips(pDC, units * 50,
             50, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
    chips -= (units * 50);
  }

  if (chips >= 25)
  {
    CBitmap* pImage = &twenty5Chip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    int units = chips / 25U;
    bltChips(pDC, units * 25,
             25, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
    chips -= (units * 25);
  }

  if (chips >= 10)
  {
    CBitmap* pImage = &tenChip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    int units = chips / 10U;
    bltChips(pDC, units * 10,
             10, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
    chips -= (units * 10);
  }

  if (chips > 0)
  {
    if (chips >= 5)
    {
      CBitmap* pImage = &fiveChip_;
      if (dwFlags & DF_CreateMask)
        pImage = &chipMask2_;

      bltChips(pDC, chips,
               5, pImage, pMask,
               xpos, ypos,
               Width_, Height_,
               chipsPerStack,
               cps,
               next_x,
               next_y,
               bufDC, memDC, transDC);
      chips -= 5;
    }

    CBitmap* pImage = &oneChip_;
    if (dwFlags & DF_CreateMask)
      pImage = &chipMask2_;

    bltChips(pDC, chips,
             1, pImage, pMask,
             xpos, ypos,
             Width_, Height_,
             chipsPerStack,
             cps,
             next_x,
             next_y,
             bufDC, memDC, transDC);
  }

  return rc;
}


CSize Chips::getChipsSize(const CChips& chips, int xpos, int ypos)
{
  CSize s(0, 0);

  if (!drawChips_ || chips == 0)
    return s;

  int numChips = getNumChipUnits(chips);
  int numStacks = 1;
  int chipsPerStack = 1;

  if (numChips < ONE_STACK)
  {
    numStacks = 1;
    chipsPerStack = numChips;
  }
  else if (numChips < TWO_STACKS)
  {
    numStacks = 2;
    chipsPerStack = (numChips / 2) + 1;
  }
  else if (numChips < THREE_STACKS)
  {
    numStacks = 3;
    chipsPerStack = (numChips / 3) + 1;
  }
  else
  { // at most 4 stacks
    numStacks = 4;
    chipsPerStack = (numChips / 4) + 1;
  }

  int next_x = xpos;
  int next_y = ypos;
  int cps = chipsPerStack;
  int topy = ypos;

  s.cx = numStacks * Width_;
  s.cy = Height_ + chipsPerStack * ChipYOffset;

  return s;
}


// Return the number of chips required to render
// 'chips' chips.
int Chips::getNumChipUnits(const CChips& cchips)
{
  UINT chips = cchips.getDollars();

  if (!drawChips_) return 0;

  UINT origChips = chips;
  int num = 0;

  if (chips >= 5000)
  {
    int units = chips / 5000U;
    num += units;
    chips -= (units * 5000);
  }

  if (chips >= 1000)
  {
    int units = chips / 1000U;
    num += units;
    chips -= (units * 1000);
  }

  if (chips >= 500)
  {
    int units = chips / 500U;
    num += units;
    chips -= (units * 500);
  }

  if (chips >= 100)
  {
    int units = chips / 100U;
    num += units;
    chips -= (units * 100);
  }

  if (chips >= 50)
  {
    int units = chips / 50U;
    num += units;
    chips -= (units * 50);
  }

  if (chips >= 25)
  {
    int units = chips / 25U;
    num += units;
    chips -= (units * 25);
  }

  if (chips >= 10)
  {
    int units = chips / 10U;
    num += units;
    chips -= (units * 10);
  }

  if (chips > 0)
  {
    if (chips >= 5)
    {
      num += 1; // one fiver
      chips -= 5;
    }

    if (chips > 0)
    {
      num += chips;
    }
  }

  //CString s;
  //s.Format("Chips: %d, paints: %d\n", origChips, num);
  //OutputDebugString(s);

  return num;
}

BOOL Chips::animate(CDC* pDC, const CChips& chips, const CPoint& newPos, const CPoint& oldPos)
{
  if (!drawChips_) return FALSE;

  BITMAP bm;
  if (chipsImage_.GetBitmap(&bm))
  {
    return Cards::DrawSprite(pDC->GetSafeHdc(),
                             newPos, oldPos,
                             bm.bmWidth, bm.bmHeight,
                             (HBITMAP)bmpSave_.GetSafeHandle(),
                             (HBITMAP)chipsImage_.GetSafeHandle(),
                             (HBITMAP)chipsMask_.GetSafeHandle());
  }
  else
    return FALSE;
}


BOOL Chips::startAnimate(CDC* pDC, const CChips& chips, const CPoint& pos, DWORD dwFlags)
{
  if (!drawChips_) return FALSE;

  BOOL rc = createAnimationBuffers(chips);

  if (!rc)
    return FALSE;

  // Create buffer for animation
  if (!bmpSave_.GetSafeHandle())
  {
    CWindowDC dcScreen(NULL);
    bmpSave_.CreateCompatibleBitmap(&dcScreen, Width_, Height_);
  }

  // Save area at 'pos'
  CDC memDC;
  rc = memDC.CreateCompatibleDC(pDC);
  if (rc)
  {
    CSize s = getAnimationBufferSize();
    CBitmap* pOldBmp = memDC.SelectObject(&bmpSave_);
    memDC.BitBlt(0, 0, s.cx, s.cy,
                 pDC, pos.x, pos.y, SRCCOPY);
    memDC.SelectObject(pOldBmp);
  }

  return rc;
}


BOOL Chips::stopAnimate(CDC* pDC, const CPoint& pos, DWORD dwFlags)
{
  if (!drawChips_) return FALSE;

  BOOL rc = TRUE;

  // Restore area at 'pos'
  CDC memDC;
  rc = memDC.CreateCompatibleDC(pDC);
  if (rc)
  {
    CSize s = getAnimationBufferSize();
    CBitmap* pOldBmp = memDC.SelectObject(&bmpSave_);
    pDC->BitBlt(pos.x, pos.y, s.cx, s.cy,
                &memDC, 0, 0, SRCCOPY);
    memDC.SelectObject(pOldBmp);
  }

  return rc;
}


BOOL Chips::bltChips(CDC* pDC, 
                     const CChips& cchips,
                     UINT chip_unit,
                     CBitmap* pImage,
                     CBitmap* pMask,
                     int xpos, int ypos,
                     int width, int height,
                     int  chipsPerStack,
                     int& cps,
                     int& next_x, // returns xpos of next chip
                     int& next_y, // returns ypos of next chip
                     CDC& bufDC,
                     CDC& memDC,
                     CDC& transDC)
{
  UINT chips = cchips.getDollars();

  int i = chips / chip_unit;
  if (i == 0)
    return TRUE; // nothing to draw

  BOOL rc = TRUE;

  CBitmap* pOldBmpBuf = bufDC.SelectObject(&bmpBuf_);
  CBitmap* pOldBmpMem = memDC.SelectObject(pImage);
  CBitmap* pOldBmpTrans = transDC.SelectObject(pMask);

  for (; i > 0; --i)
  {
    // Copy current background to buf
    
    bufDC.BitBlt(0, 0, width, height,
                 pDC, next_x, next_y, SRCCOPY);

    // Transparent blit in to buf
  
    bufDC.BitBlt(0, 0, width, height,
                 &transDC, 0, 0, SRCAND);
    bufDC.BitBlt(0, 0, width, height,
                 &memDC, 0, 0, SRCINVERT);

    // Now blt the buffer to target dc
    pDC->BitBlt(next_x, next_y, width, height,
                &bufDC, 0, 0, SRCCOPY);
  
    // blt next chip a couple of pixels up
    next_y -= ChipYOffset;

    if (--cps <= 0)
    {  // time to move to next stack
      next_x += width;
      next_y = ypos;
      cps = chipsPerStack;
    }
  }

  if (cps <= 0)
  { // time to move to next stack
    next_x += width;
    next_y = ypos;
    cps = chipsPerStack;
  }

  // Clean up
  bufDC.SelectObject(pOldBmpBuf);
  memDC.SelectObject(pOldBmpMem);
  transDC.SelectObject(pOldBmpTrans);

  return rc;
}


void Chips::sizeChips(const CChips& cchips,
                      UINT chip_unit,
                      int xpos, int ypos,
                      int width, int height,
                      int  chipsPerStack,
                      int& cps,
                      int& next_x, // returns xpos of next chip
                      int& next_y, // returns ypos of next chip
                      int& top_y)  // returns ypos of highest chip
{
  UINT chips = cchips.getDollars();

  int i = chips / chip_unit;
  if (i == 0)
    return; // nothing to draw

  BOOL rc = TRUE;

  for (; i > 0; --i)
  {
    // blt next chip a couple of pixels up
    next_y -= ChipYOffset;

    top_y = next_y;

    if (cps-- <= 0)
    {  // time to move to next stack
      next_x += width;
      next_y = ypos;
      cps = chipsPerStack;
    }
  }
}

// Blit chips as one stack
BOOL Chips::bltChips(CDC* pDC, 
                     int xpos, int ypos,
                     int width, int height,
                     int chipx, int chipy,
                     CDC& bufDC,
                     CDC& memDC,
                     CDC& transDC)
{
  return TRUE;
}


// Blit each chip separately; first chip
// is blt bottom-most, next chip on top of it.
BOOL Chips::bltChips(CDC* pDC, 
                     const CChips& cchips,
                     UINT chip_unit,
                     CBitmap* pImage,
                     int xpos, int ypos,
                     int width, int height,
                     int& next_y,
                     CDC& bufDC,
                     CDC& memDC,
                     CDC& transDC)
{
  UINT chips = cchips.getDollars();

  int i = chips / chip_unit;
  if (i == 0)
    return TRUE; // nothing to draw

  BOOL rc = TRUE;

  next_y = ypos;

  CBitmap* pOldBmpBuf = bufDC.SelectObject(&bmpBuf_);
  CBitmap* pOldBmpMem = memDC.SelectObject(pImage);
  CBitmap* pOldBmpTrans = transDC.SelectObject(&chipMask_);

  for (; i > 0; --i)
  {
    // Copy current background to buf
    
    bufDC.BitBlt(0, 0, width, height,
                 pDC, xpos, next_y, SRCCOPY);

    // Transparent blit in to buf
  
    bufDC.BitBlt(0, 0, width, height,
                 &transDC, 0, 0, SRCAND);
    bufDC.BitBlt(0, 0, width, height,
                 &memDC, 0, 0, SRCINVERT);

    // Now blt the buffer to target dc
    pDC->BitBlt(xpos, next_y, width, height,
                &bufDC, 0, 0, SRCCOPY);
  
    // blt next chip a couple of pixels up
    next_y -= ChipYOffset;
  }

  // Clean up
  bufDC.SelectObject(pOldBmpBuf);
  memDC.SelectObject(pOldBmpMem);
  transDC.SelectObject(pOldBmpTrans);

  return rc;
}

BOOL Chips::createAnimationBuffers(const CChips& chips)
{
  if (chips == chips_)
  {  // The same amount of chips was animated last time,
     // reuse animation buffers
     return TRUE;
  }
  else
  {  // Must recreate animation buffers if
     // image will be bigger than previous animation
     chips_ = chips;
     CSize s = getChipsSize(chips, 0, 0);

     // SANITY CHECK
     if (s.cx > 300)
       s.cx = 300;
     if (s.cy > 300)
       s.cy = 300;

     bool recreate = false;
     BITMAP bm;
     memset(&bm, 0, sizeof(bm));

     if (!chipsImage_.GetSafeHandle())
     {
       bm.bmWidth = s.cx;
       bm.bmHeight = s.cy;
       recreate = true;
     }
     else
     {
       chipsImage_.GetBitmap(&bm);
       if (bm.bmWidth != s.cx || bm.bmHeight != s.cy)
         recreate = true;
       else
         recreate = false;
     }

     CWindowDC dcScreen(NULL);
     if (recreate)
     {
       chipsImage_.DeleteObject();
       chipsMask_.DeleteObject();
       bmpSave_.DeleteObject();

       if (!chipsImage_.CreateCompatibleBitmap(&dcScreen, s.cx, s.cy) ||
           !chipsMask_.CreateCompatibleBitmap(&dcScreen, s.cx, s.cy) ||
           !bmpSave_.CreateCompatibleBitmap(&dcScreen, s.cx, s.cy))
       {
         return FALSE;
       }

       chipsImage_.GetBitmap(&bm);
     }

     CDC dcTmp;
     dcTmp.CreateCompatibleDC(&dcScreen);

     // Create the image
     CBitmap* pOldBmp = dcTmp.SelectObject(&chipsImage_);
     dcTmp.PatBlt(0, 0, bm.bmWidth, bm.bmHeight, BLACKNESS);
     drawChips(&dcTmp, chips, 0, bm.bmHeight - Height_, 0);

// Debugging:
//     dcScreen.BitBlt(0, 0, bm.bmWidth, bm.bmHeight, &dcTmp,
//                     0, 0, SRCCOPY);

     // Create the mask
     dcTmp.SelectObject(&chipsMask_);
     dcTmp.PatBlt(0, 0, bm.bmWidth, bm.bmHeight, WHITENESS);
     drawChips(&dcTmp, chips, 0, bm.bmHeight - Height_, DF_CreateMask);

// Debugging:
//     dcScreen.BitBlt(bm.bmWidth + 2, 0, bm.bmWidth, bm.bmHeight, &dcTmp,
//                     0, 0, SRCCOPY);

     // Clean up
     dcTmp.SelectObject(pOldBmp);
   }

  return TRUE;
}

CSize Chips::getAnimationBufferSize()
{
  BITMAP bm;
  if (chipsImage_.GetBitmap(&bm))
  {
    return CSize(bm.bmWidth, bm.bmHeight);
  }
  else
  {
    return CSize(0, 0);
  }
}

} // Base
