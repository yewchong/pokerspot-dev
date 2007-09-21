#ifndef __base_transparentbitmap_h__
#define __base_transparentbitmap_h__

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

namespace Base
{

class TransparentBitmap
{
public:
  enum BltFlag {
    FlipX = 1,
    FlipY = 2
  };

public:
  TransparentBitmap()
    : size_(0, 0) {}
  
  BOOL LoadBitmap(UINT nIDImage, UINT nIDMask)
  {
    BOOL rc = bmpImage_.LoadBitmap(nIDImage) &&
              bmpMask_.LoadBitmap(nIDMask);
    if (rc)
    {
      BITMAP bm; 
      if (GetBitmap(&bm))
      {
        size_.cx = bm.bmWidth;
        size_.cy = bm.bmHeight;
      } 
    }

    return rc;
  }

  void DeleteObject()
  {
    bmpImage_.DeleteObject();
    bmpMask_.DeleteObject();
    size_.cx = size_.cy = 0;
  }

  BOOL GetBitmap(BITMAP* pBm)
  {
    return bmpImage_.GetBitmap(pBm);
  }

  CSize GetSize() const
  {
    return size_;
  }

  HBITMAP GetSafeHandle() const { return (HBITMAP)bmpImage_.GetSafeHandle(); }

  BOOL TransBlt(CDC*, int x, int y, DWORD dwFlags = 0/*BltFlag*/);
  BOOL TransBlt(CDC* pDC, const CPoint& pt, DWORD dwFlags = 0/*BltFlag*/)
  {
      return TransBlt(pDC, pt.x, pt.y, dwFlags);
  }

private:
  CBitmap bmpImage_,
          bmpMask_;
  CSize   size_;
};


} // Base

#endif