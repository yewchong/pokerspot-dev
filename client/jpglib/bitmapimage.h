#ifndef CB__BITMAPIMAGE_H
#define CB__BITMAPIMAGE_H

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
//  Title: BitmapImage Class Definitions
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//    The BitmapImage class is intended to be a neutral intermediate format
//    for storing decompressed images. This class can manage 1, 2, 4, 8 or
//    24-bit images. For 24-bit images the data is stored as RGB triples
//    within the main data buffer. For all other types a color map is used
//    and the image data contains indices into the color map. Sample values are
//    assumed to be in the range 0..255.
//
//  Windows Notes:
//
//    For the sake of "efficiency" this class has been optimized for use on
//    the "Windows" family. The folling oddities are a result of
//    "windowsisms":
//
//    o The data for 24-bitmaps is stored in BGR order rather than RGB.
//      To change this for your system redefine "RedOffset", "GreenOffset",
//      and "BlueOffset".
//
//    o For whatever reason, Windows expects bitmaps to be stored bottom
//      up rather than top down. The first row in the bitmap data is the
//      bottom row in the image. To change behavoir this for your system
//      redefine the implementation of the [] operator.
//
//    o Windows expects the length of all image rows to be rounded up to the
//      nearest four bytes. To change this behavior redefine the value for
//     "RowRounding".
//
//  Debugging Notes:
//
//    Two methods for accessing pixel data within the image are implemented
//    by default range checking is only performed on rows. If the
//    preprocessor symbol CHECK_RANGE is defined then range check is
//    performed on columns as well.
//
//    While the abandonment of range checking here is contrary to the
//    principles followed elsewhere, this is a place where the
//    performance benefit is worth the lack of safety.
//

#include <iostream>
#include "systemspecific.h"

namespace Colosseum
{

class BitmapImage ;
class BitmapImageCoder ;


typedef void (*PROGRESSFUNCTION)(BitmapImageCoder &coder,
                                 void *data,
                                 unsigned int currentpass,
                                 unsigned int passcount,
                                 unsigned int progress,
                                 bool &cancel) ;

//
// Description:
//
//   Common class for all encoders and decoders.
//
class BitmapImageCoder
{
public:
  BitmapImageCoder () : progress_function (0), progress_data (0) {}
  BitmapImageCoder (const BitmapImageCoder &source) ;
  virtual ~BitmapImageCoder () {}
  BitmapImageCoder &operator=(const BitmapImageCoder &source) ;

  void setProgressFunction (PROGRESSFUNCTION, void *) ;
  // Function to force an update of image data.
  virtual void updateImage () {}

protected:
  PROGRESSFUNCTION progress_function ;
  void *progress_data ;
} ;

//
// Description:
//
//  Common decoder class.
//
class BitmapImageDecoder : public BitmapImageCoder
{
public:
  virtual ~BitmapImageDecoder () {}
  virtual void readImageFile (const std::string &filename, BitmapImage &) = 0 ;
} ;

//
// Description:
//
//  Common encoder class.
//
class BitmapImageEncoder : public BitmapImageCoder
{
public:
  virtual ~BitmapImageEncoder () {}
  virtual void writeImageFile (const std::string &, const BitmapImage &) = 0 ;
} ;

//
// Description:
//
//  The BitmapImage class represents a bitmap image. This class is 
//  implemented to allow it to be displayed easily on Windoze.
//
class DLL_DECLARE_ BitmapImage
{
public:

#include "beginpackbyte.h"
  struct Pixel
  {
    // The BGRa order is assumed in the BMP encoder
    UBYTE1 blue ;
    UBYTE1 green ;
    UBYTE1 red ;
    UBYTE1 alpha ;
  } ;
#include "endpackbyte.h"

  // Required Member Functions
  BitmapImage () ;
  BitmapImage (const BitmapImage &) ;
  BitmapImage (unsigned int initialwidth, 
	           unsigned int initialheight, 
			   Pixel background) ;
  virtual ~BitmapImage () ;
  BitmapImage &operator=(const BitmapImage &) ;

  void setSize (unsigned int width, unsigned int height) ;

  const Pixel &operator[](unsigned int) const ;
  Pixel &operator[](unsigned int)  ;

  // Function to reset the image to empty
  void clearImage () ;

  // Function to return information about the image.
  UBYTE1 *imageData() ;
  unsigned int getWidth () const ;
  unsigned int getHeight () const ;

  void mergeImage (const BitmapImage &image, int xpos = 0, int ypos = 0) ;

protected:
  void doCopy (const BitmapImage &) ;

private:
  unsigned int image_width ;
  unsigned int image_height ;
  unsigned int image_size ; // width * height
  Pixel *image_data ;
} ;

//
// Description:
//
//  This function returns the image width in pixels.
//
// Return Value:
//
//  The image width.
//
inline unsigned int BitmapImage::getWidth () const
{
  return image_width ;
}

//
// Description:
//
//  This function returns the image height in pixels.
//
// Return Value:
//
//  The image height.
//
inline unsigned int BitmapImage::getHeight () const
{
 return image_height ;
}
//
// Description:
//
//  This definition of the [] operator returns a pointer to
//  a row in the image pixel data.
//
// Return Value:
//
//  A pointer to a pixel row.
//
inline const BitmapImage::Pixel &BitmapImage::operator[](unsigned int xx) const
{
  return image_data [xx] ;
}
inline BitmapImage::Pixel &BitmapImage::operator[](unsigned int xx)
{
  return image_data [xx] ;
}

//
// Description:
//
//  This is the common class for all exceptions thrown by the graphics classes.
//
class GraphicsException : public std::exception
{
private:
  std::string message ;
public:
  GraphicsException (const std::string &msg) : message (msg) {} ;
  GraphicsException (const GraphicsException &source) : message (source.message) {}
  GraphicsException &operator=(const GraphicsException &source)
    {
      if (&source != this)
      {
        message = source.message ;
      }
      return *this ;
    }
  virtual const char *what () const throw () { return message.c_str () ; }
} ;

//
// Description:
//
//  GraphicsAbort is thrown by graphics classes when the user requests
//  to abort an operation. This exception does not represent an error condition.
//
class GraphicsAbort
{
} ;

} // End Namespace

#endif
