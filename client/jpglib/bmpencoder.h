#ifndef CB__BMBENCODER_H
#define CB__BMPENCODER_H

//
//  Title:  Windows Bitmap Encoder Class
//
//  Copyright 1997-1999, Colosseum Builders, Inc.
//  All rights reserved.
//
//  Colosseum Builders, Inc. makes no warranty, expressed or implied
//  with regards to this software. It is provided as is.
//
//  See the README.TXT file that came with this software for restrictions
//  on the use and redistribution of this file or send E-mail to
//  info@colosseumbuilders.com
//
//  Author:  John M. Miano (miano@colosseumbuilders.com)
//
//  Date:    April 15, 1999
//
//  Version: 2
//
//  Description:
//
//    A class for encoding a BitmapImage object into a Windows BMP file.
//
//  Revision History:
//
//

#include <iostream>

#include "bitmapimage.h"

namespace Colosseum
{

class BmpEncoder : public BitmapImageEncoder
{
public:
  BmpEncoder () ;
  BmpEncoder (const BmpEncoder &) ;
  virtual ~BmpEncoder () {}
  BmpEncoder &operator=(const BmpEncoder &) ;
  virtual void writeImage (std::ostream &, const BitmapImage &) ;
  virtual void writeImageFile (const std::string &, const BitmapImage &) ;

  class BmpError ;
  class BmpNotSupported ;

  void setCreateAlphaChannel (bool) ;
  bool getCreateAlphaChannel () const ;

  // Set the bits per pixel to 24 to allow the image to be
  // read by most applications.
  void setBitsPerPixel (unsigned int) ;
  unsigned int getBitsPerPixel () const ;

private:
  bool create_alpha_channel ;
  unsigned int bits_per_pixel ;
  void callProgressFunction (unsigned int) ;
} ;

class BmpEncoder::BmpError : public GraphicsException
{
public:
  BmpError (const std::string &msg) : GraphicsException (msg) {} 
} ;

class BmpEncoder::BmpNotSupported : public BmpEncoder::BmpError
{
public:
  BmpNotSupported (const std::string &msg) : BmpError (msg) {} 
} ;

} // End Namespace Colosseum

#endif
