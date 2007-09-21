#ifndef __base_jpgimage_h__
#define __base_jpgimage_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class JPGImage
  Created  : 03/18/2000

  OVERVIEW : A class for loading & displaying JPG images.

 ****************************************************************************/

namespace Base
{

  struct JPGImpl;

  class JPGImage
  {
  public:
    static JPGImage* LoadImage(const char* filename);
    ~JPGImage();

    BOOL drawImage(HDC);
    BOOL drawImage(HDC, const CRect&);
    CSize getSize() const;

  private:
    JPGImage();
    JPGImpl* pImpl_;
    CBitmap bmp_;
    CSize   size_;
  };

};

#endif