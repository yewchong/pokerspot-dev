#ifndef CB__JFIF_H
#define CB__JFIF_H
//
// Copyright (c) 1999 Colosseum Builders, Inc.
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
//  Title:  JPEG Definitions and Utility Functions
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//


//
//  JFIF Definitions
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  These definitions match version 1.02 of the
//  "JPEG File Interchange Format Specification" by Eric Hamilton
//

#include "jpgpvt.h"

namespace ColosseumPrivate
{
const int ScaleFactor = 12 ;
const int ScaleValue = (1<<ScaleFactor) ;
const int Rounding = (1<<(ScaleFactor-1)) ;

inline int Scaled (double value)
{
  return static_cast<int>(ScaleValue * value) ;
}

#include "beginpackbyte.h"
struct JfifHeader
{
  UBYTE2 length ;
  UBYTE1 identifier [5] ;
  UBYTE1 version [2] ;
  UBYTE1 units ;
  UBYTE2 xdensity ;
  UBYTE2 ydensity ;
  UBYTE1 xthumbnail ;
  UBYTE1 ythumbnail ;
  // 3 * xthumbnail * ythumbnail bytes of thumbnail data follow.
} ;
#include "endpackbyte.h"

#include "beginpackbyte.h"
struct JfifExtension
{
  UBYTE2 length ;
  UBYTE1 identifier [5] ;
  UBYTE1 extension_code ;
  enum { JpegThumbnail = 0x10,
         OneByteThumbnail = 0x11,
         ThreeByteThumbnail = 0x13, } ;
  // Extension Data follows
} ;
#include "endpackbyte.h"

// To implement 12-bit data the return values would have to be
// at least 12 bits wide.

// Functions for YCbCr/RGB colorspace conversion
extern JPEGSAMPLE YCbCrToR (int yy, int cb, int cr) ;
extern JPEGSAMPLE YCbCrToG (int yy, int cb, int cr) ;
extern JPEGSAMPLE YCbCrToB (int yy, int cb, int cr) ;

extern JPEGSAMPLE RgbToY (JPEGSAMPLE red, JPEGSAMPLE green, JPEGSAMPLE blue) ;
extern JPEGSAMPLE RgbToCb (JPEGSAMPLE red, JPEGSAMPLE green, JPEGSAMPLE blue) ;
extern JPEGSAMPLE RgbToCr (JPEGSAMPLE red, JPEGSAMPLE green, JPEGSAMPLE blue) ;

//
// The inlining of color conversion saves 7% in testing.
//
#define INLINECOLORCONVERSION
#if defined (INLINECOLORCONVERSION)

extern const long CrRTable [256] ;
extern const long CbGTable [256] ;
extern const long CrGTable [256] ;
extern const long CbBTable [256] ;

//
//  Tried using an array to limit the values to 0...255. Produced no improvement,
//  probably due to cache overflow.
//

inline JPEGSAMPLE YCbCrToR (int yy, int /*cb*/, int cr)
{
  long result = yy + CrRTable [cr] ;
  if (result < JPEGMINSAMPLEVALUE)
    result = JPEGMINSAMPLEVALUE ;
  else if (result > JPEGMAX8BITSAMPLEVALUE)
    result = JPEGMAX8BITSAMPLEVALUE ;
  return static_cast<JPEGSAMPLE>(result) ;
}

inline JPEGSAMPLE YCbCrToG (int yy, int cb, int cr)
{
  const int shift = 16 ;
  const int rounding = 1 << (shift-1) ;
  long result = yy - ((CbGTable [cb] + CrGTable [cr] + rounding) >> shift) ;

  if (result < JPEGMINSAMPLEVALUE)
    result = JPEGMINSAMPLEVALUE ;
  else if (result > JPEGMAX8BITSAMPLEVALUE)
    result = JPEGMAX8BITSAMPLEVALUE ;
  return static_cast<JPEGSAMPLE> (result) ;
}

inline JPEGSAMPLE YCbCrToB (int yy, int cb, int /*cr*/)
{
  long result = yy + CbBTable [cb] ;
  if (result < JPEGMINSAMPLEVALUE)
    result = JPEGMINSAMPLEVALUE ;
  else if (result > JPEGMAX8BITSAMPLEVALUE)
    result = JPEGMAX8BITSAMPLEVALUE ;
  return static_cast<JPEGSAMPLE> (result) ;
}

inline JPEGSAMPLE RgbToY (JPEGSAMPLE red, JPEGSAMPLE green, JPEGSAMPLE blue)
{
  int result = (Scaled (0.299) * red
             + Scaled (0.587) * green + Scaled (0.114) * blue
             + Rounding) >> ScaleFactor ;
  if (result > JPEGMAX8BITSAMPLEVALUE)
  	result = JPEGMAX8BITSAMPLEVALUE ;
  else if (result < JPEGMINSAMPLEVALUE)
  	result = JPEGMINSAMPLEVALUE ;
  return static_cast<JPEGSAMPLE>(result) ;
}

inline JPEGSAMPLE RgbToCb (JPEGSAMPLE red, JPEGSAMPLE green, JPEGSAMPLE blue)
{
  int result = ((JPEG8BITMIDPOINTSAMPLEVALUE<<ScaleFactor)
             + Rounding - Scaled (0.1687) * red
             - Scaled (0.3313) * green + Scaled (0.5) * blue) >> ScaleFactor  ;
  if (result > JPEGMAX8BITSAMPLEVALUE)
  	result = JPEGMAX8BITSAMPLEVALUE ;
  else if (result < JPEGMINSAMPLEVALUE)
  	result = JPEGMINSAMPLEVALUE ;
  return static_cast<JPEGSAMPLE>(result) ;
}

inline JPEGSAMPLE RgbToCr (JPEGSAMPLE red, JPEGSAMPLE green, JPEGSAMPLE blue)
{
  int result = ((JPEG8BITMIDPOINTSAMPLEVALUE<<ScaleFactor)
             + Rounding + Scaled (0.5) * red
             - Scaled (0.4187) * green - Scaled (0.0813) * blue) >> ScaleFactor ;
  if (result > JPEGMAX8BITSAMPLEVALUE)
  	result = JPEGMAX8BITSAMPLEVALUE ;
  else if (result < JPEGMINSAMPLEVALUE)
  	result = JPEGMINSAMPLEVALUE ;
  return static_cast<JPEGSAMPLE>(result) ;
}
#endif

} // End Namespace ColosseumPrivate

#endif
