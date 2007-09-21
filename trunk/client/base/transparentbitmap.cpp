/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class TransparentBitmap
  Created  : 1.11.1999

  OVERVIEW : Implements a transparent bitmap.

             Transparency is achieved by using two bitmaps:
             the image and the mask. The image bitmap contains
             the image of interest with transparent areas black.
             The mask is a black and white bitmap that has the
             image area black and transparent areas white.a 
             
             The bitmaps are blt in succession with appropriate
             ROP codes to make it look transparent.

 ****************************************************************************/

#include "stdafx.h"
#include "base/transparentbitmap.h"

namespace Base
{

BOOL TransparentBitmap::TransBlt(CDC* pDC, int xpos, int ypos,
                                 DWORD dwFlags)
{
  int targetx = 0, targety = 0, flipx = 1, flipy = 1;
  if (dwFlags & FlipX)
  {
    targetx = size_.cx - 1;
    flipx = -1;
  }
  if (dwFlags & FlipY)
  {
    targety = size_.cy - 1;
    flipy = -1;
  }

  CDC bufDC, memDC;

  BOOL rc = bufDC.CreateCompatibleDC(pDC) &&
            memDC.CreateCompatibleDC(pDC);

  CBitmap bmpBuf;
  if (rc)
    rc = bmpBuf.CreateCompatibleBitmap(pDC, size_.cx, size_.cy);

  if (rc)
  {
    // Construct the image in buffer and then blit
    // it to target DC

    // Copy affect area to bufDC  
    CBitmap* pOldBmpBuf = bufDC.SelectObject(&bmpBuf);
    bufDC.BitBlt(0, 0, size_.cx, size_.cy,
                 pDC, xpos, ypos, SRCCOPY);
    
    // Construct transparent image in bufDC
    CBitmap* pOldBmpMem = memDC.SelectObject(&bmpMask_);

    bufDC.StretchBlt(targetx, targety,
                     flipx * size_.cx, flipy * size_.cy,
                     &memDC, 0, 0, size_.cx, size_.cy,
                     SRCAND);

    memDC.SelectObject(&bmpImage_);
    bufDC.StretchBlt(targetx, targety,
                     flipx * size_.cx, flipy * size_.cy,
                     &memDC, 0, 0, size_.cx, size_.cy,
                     SRCINVERT);

    // Now blt the complete buffer to target dc
    pDC->BitBlt(xpos, ypos, size_.cx, size_.cy,
                &bufDC, 0, 0, SRCCOPY);

    // Clean up
    bufDC.SelectObject(pOldBmpBuf);
    memDC.SelectObject(pOldBmpMem);
  }  

  return rc;
}


} // Base

