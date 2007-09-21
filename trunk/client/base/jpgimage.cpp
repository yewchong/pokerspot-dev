/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class JPGImage
  Created  : 03/18/2000

  OVERVIEW : A class for loading & displaying JPG images.

 ****************************************************************************/

#include "stdafx.h"
#include "base/jpgimage.h"
#include "jpglib/imagetype.h"
#include "jpglib/bitmapimage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Link in the jpglib import library
#ifndef DISABLE_JPGSUPPORT_
#ifdef PSPOT_TABLE_MODULE_
#pragma comment(lib, "jpglib/bin/jpglib.lib")
#else
#pragma comment(lib, "../jpglib/bin/jpglib.lib")
#endif
#endif

//#define TOURNAMENT_TESTING_ 1

namespace Base
{
#ifndef DISABLE_JPGSUPPORT_
  struct JPGImpl
  {
    Colosseum::BitmapImage image_;

    CSize getSize() const
    {
#ifdef TOURNAMENT_TESTING_
      return CSize(640, 350);
#else
      CSize s(image_.getWidth(), image_.getHeight());
      return s;
#endif
    }

    BOOL drawImage(HDC hdc)
    {
#ifdef TOURNAMENT_TESTING_
      CRect r(CPoint(0, 0), getSize());
      ::FillRect(hdc, &r, (HBRUSH)::GetStockObject(GRAY_BRUSH));
      return TRUE;
#endif

      BOOL rc = FALSE;

      static BOOL showError = TRUE;

      if (image_.getWidth() > 0 && image_.getHeight() > 0)
      {
        BITMAPINFOHEADER hdr;
        memset(&hdr, 0, sizeof(hdr));
        hdr.biSize = sizeof(hdr);
        hdr.biWidth = image_.getWidth();
        hdr.biHeight = -image_.getHeight();
        hdr.biPlanes = 1;
        hdr.biBitCount = 32;
        hdr.biCompression = BI_RGB;
        hdr.biSizeImage = 0;

        rc = ::SetDIBitsToDevice(
                           hdc,
                           0,
                           0,
                           image_.getWidth(),
                           image_.getHeight(),
                           0,
                           0,
                           0,
                           image_.getHeight(),
                           &image_[0],
                           reinterpret_cast<const BITMAPINFO*>(&hdr),
                           DIB_RGB_COLORS);

        if (rc == 0 && showError)
        {
          showError = FALSE;
          CString s;
          s.Format("JpgImage::drawImage: SetDIBitsToDevice failed, hdc: %d error: %d", hdc, GetLastError());
          AfxMessageBox(s);
        }
      }
      else if (showError)
      {
        showError = FALSE;
        CString s;
        s.Format("JpgImage::drawImage: invalid image size: %d, %d",
                 image_.getWidth(), image_.getHeight());
        AfxMessageBox(s);
      }
      return rc;
    }
  };

#else

  struct JPGImpl
  {
    CSize getSize() const
    {
      return CSize(100, 100);
    }

    BOOL drawImage(HDC hdc)
    {
      return TRUE;      
    }
  };

#endif


  JPGImage::JPGImage()
    : pImpl_(0), size_(0, 0)
  {
    pImpl_ = new JPGImpl();
  }
  
  
  JPGImage::~JPGImage()
  {
    delete pImpl_;
    pImpl_ = NULL;
  }


  JPGImage* JPGImage::LoadImage(const char* filename)
  {
    JPGImage* pImg = new JPGImage();
    if (!pImg || !pImg->pImpl_)
    {
      CString s;
      s.Format("Out of memory when loading jpg image: %s", filename);
      AfxMessageBox(s);
    }

#ifdef TOURNAMENT_TESTING_
    return pImg; 
#endif

    if (pImg)
    {
      if (pImg->pImpl_)
      {
        try
        { 
          CWaitCursor wait;

          // The JPG library does the real work
          Colosseum::ImageType type =
            Colosseum::ReadImage(filename, pImg->pImpl_->image_, 0, 0);
          if (type == Colosseum::UnknownImage)
          {
            delete pImg;
            pImg = 0;
            CString s;
            s.Format("Loading image %s failed.", filename);
            AfxMessageBox(s);
          }

        }
        catch (std::exception& e)
        {
          std::string s = e.what();
          delete pImg;
          pImg = 0;
          CString msg;
          msg.Format("Loading %s failed: %s", filename, s.c_str());
          AfxMessageBox(msg);
        }
      }
    }
    return pImg;
  }


  // Draw the image in its real size.
  BOOL JPGImage::drawImage(HDC hdc)
  {
    BOOL rc = FALSE;

    CDC* pDC = CDC::FromHandle(hdc);

    if (bmp_.GetSafeHandle() == NULL && pImpl_ != NULL)
    { // First draw, create bitmap out of the
      // jpg image. Subsequent calls will use
      // the bitmap.
      size_ = pImpl_->getSize();
      if (size_.cx > 0 && size_.cy > 0)
      {
        CDC dcMem;
        dcMem.CreateCompatibleDC(pDC);
        bmp_.CreateCompatibleBitmap(pDC, size_.cx, size_.cy);

        // Draw the JPG image onto the bitmap
        CBitmap* pOldBmp = dcMem.SelectObject(&bmp_);
        pImpl_->drawImage(dcMem.GetSafeHdc());
        dcMem.SelectObject(pOldBmp);

        // From now on, the bitmap will be used
        delete pImpl_;
        pImpl_ = NULL;
      }
    }

    if (bmp_.GetSafeHandle())
    {
      CDC dcMem;
      if (dcMem.CreateCompatibleDC(pDC))
      {
        CBitmap* pOldBmp = dcMem.SelectObject(&bmp_);
        pDC->BitBlt(0, 0, size_.cx, size_.cy, &dcMem, 0, 0, SRCCOPY);
        dcMem.SelectObject(pOldBmp);
      }
    }

    return rc;
  }


  // Draw the image so it is scaled into 'rect'.
  BOOL JPGImage::drawImage(HDC hdc, const CRect& rect)
  {
    BOOL rc = FALSE;

    CDC* pDC = CDC::FromHandle(hdc);

    if (bmp_.GetSafeHandle() == NULL && pImpl_ != NULL)
    { // First draw, create bitmap out of the
      // jpg image. Subsequent calls will use
      // the bitmap.
      size_ = pImpl_->getSize();
      if (size_.cx > 0 && size_.cy > 0)
      {
        CDC dcMem;
        dcMem.CreateCompatibleDC(pDC);
        bmp_.CreateCompatibleBitmap(pDC, size_.cx, size_.cy);

        // Draw the JPG image onto the bitmap
        CBitmap* pOldBmp = dcMem.SelectObject(&bmp_);
        pImpl_->drawImage(dcMem.GetSafeHdc());
        dcMem.SelectObject(pOldBmp);

        // From now on, the bitmap will be used
        delete pImpl_;
        pImpl_ = NULL;
      }
    }

    if (bmp_.GetSafeHandle())
    {
      CDC dcMem;
      if (dcMem.CreateCompatibleDC(pDC))
      {
        CBitmap* pOldBmp = dcMem.SelectObject(&bmp_);
        pDC->StretchBlt(rect.left, rect.top,
                        rect.Width(), rect.Height(), &dcMem,
                        0, 0, size_.cx, size_.cy, SRCCOPY);
        dcMem.SelectObject(pOldBmp);
      }
    }

    return rc;
  }

  CSize JPGImage::getSize() const
  {    
    CSize s(size_);
    if (pImpl_)
      s = pImpl_->getSize();
    return s;
  }

};

