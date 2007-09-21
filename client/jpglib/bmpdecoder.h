#ifndef CB__BMPDECODER_H
#define CB__BMPDECODER_H
//
// Copyright (c) 1997,1998 Colosseum Builders, Inc.
// All rights reserved.
//
// Colosseum Builders, Inc. makes no warranty, expressed or implied
// with regards to this software. It is provided as is.
//
// See the README.TXT file that came with this software for restrictions
// on the use and redistribution of this file or send E-mail to
// info@colosseumbuilders.com
//

//
//  BMP Decoder Library.
//
//  Title:   BmpDecoder Class Implementation
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class decodes Windows BMP file.
//
//

#include <iosfwd>
#include <windows.h>
#include "bitmapimage.h"

namespace Colosseum
{

class BmpDecoder : public BitmapImageDecoder
{
public:
  BmpDecoder () ;
  virtual ~BmpDecoder () ;
  BmpDecoder (const BmpDecoder &) ;
  BmpDecoder &operator=(const BmpDecoder &) ;

  virtual void readImageFile (const std::string &, BitmapImage &) ;
  virtual void readImage (std::istream &, BitmapImage &) ;

  class BmpError ;
  
private:
  void doCopy (const BmpDecoder &) ;

  void readFractionalByteData (std::istream &strm,
                               unsigned int bitcount,
                               int height, unsigned int width,
                               BitmapImage &image) ;
  void readRle4 (std::istream &strm ,
                 int height, int width,
                 BitmapImage &image) ;
  void readRle8 (std::istream &strm ,
                 int height, int width,
                 BitmapImage &image) ;
  void read8BitData (std::istream &strm ,
                     int height, int width,
                     BitmapImage &image) ;
  void read16BitData (std::istream &strm ,
                      int height, int width,
                      BitmapImage &image,
                      unsigned long redmask, unsigned long greenmask,
                      unsigned long bluemask, unsigned long alphamask) ;
  void read24BitData (std::istream &strm ,
                      int height, int width,
                      BitmapImage &image) ;
  void read32BitData (std::istream &strm ,
                      int height, int width,
                      BitmapImage &image,
                      unsigned long redmask, unsigned long greenmask,
                      unsigned long bluemask, unsigned long alphamask) ;

  unsigned int readOs2ColorTable (std::istream &strm, unsigned int colorcount) ;
  unsigned int readColorTable (std::istream &strm, unsigned int colorcount) ;

  void callProgressFunction (unsigned int percent) ;

  RGBQUAD color_table [256] ;
  unsigned int color_table_size ;
} ;

class BmpDecoder::BmpError : public GraphicsException
{
public:
  BmpError (const std::string &msg) : GraphicsException (msg) {} ;
  BmpError (const BmpError &source) : GraphicsException (source) {} ;
} ;

}

#endif
