#ifndef CB__PNGPVT_H
#define CB__PNGPVT_H
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
//  Title:  PNG Encoder/Decoder private definitions
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//


#include "systemspecific.h"

namespace ColosseumPrivate
{
const int PNGMAX8BITSAMPLEVALUE = 255 ;

// Length of the PNG signature.
const int PngSignatureSize = 8 ;
const UBYTE1 PngSignature [PngSignatureSize] =
  {
    137, 80, 78, 71, 13, 10, 26, 10,
  } ;


enum PngColorType
{
  Grayscale = 0,
  RGB = 2,
  Palette = 3,
  GrayscaleAlpha = 4,
  RGBAlpha = 6,
} ;

// Filter type defintiions
enum  PngFilterType
{
  FILTERNONE = 0,
  FILTERSUB = 1,
  FILTERUP = 2,
  FILTERAVERAGE = 3,
  FILTERPAETH = 4,
} ;

// Physical layout of the IDAT header.
#include "beginpackbyte.h"
struct PngImageHeader
{
  UBYTE4 width ;
  UBYTE4 height ;
  UBYTE1 bitdepth ;
  UBYTE1 colortype ;
  UBYTE1 compressionmethod ;
  UBYTE1 filtermethod ;
  UBYTE1 interlacemethod ;
} ;
#include "endpackbyte.h"

// Physical Layout of a cHRM chunk
#include "beginpackbyte.h"
struct PngChromaticitiesData
{
  UBYTE4 whitepointx ;
  UBYTE4 whitepointy ;
  UBYTE4 redx ;
  UBYTE4 redy ;
  UBYTE4 greenx ;
  UBYTE4 greeny ;
  UBYTE4 bluex ;
  UBYTE4 bluey ;
} ;
#include "endpackbyte.h"

// Physical layout of a pPYs chucnk
#include "beginpackbyte.h"
struct PngPixelDimensions
{
  UBYTE4 pixelsx ;
  UBYTE4 pixelsy ;
  UBYTE1 unit ;
} ;
#include "endpackbyte.h"

// Physical layout of a tIME chunk
#include "beginpackbyte.h"
struct PngTimeData
{
  UBYTE2 year ;
  UBYTE1 month ;
  UBYTE1 day ;
  UBYTE1 hour ;
  UBYTE1 minute ;
  UBYTE1 second ;
} ;
#include "endpackbyte.h"

//
//  Description:
//    Path predictor function defined in section 6.6 of the PNG standard.
//
//  Parameters:
//    left:  The pixel value for the value to the left of the current pixel.
//    above: The value for the pixel above the current pixel.
//    upperleft: The value for the pixel diagonally above and to the right
//                of the current pixel.
//
static inline unsigned int  PaethPredictor (UBYTE1 left,
                                            UBYTE1 above,
                                            UBYTE1 upperleft)
{
  int pp = left + above - upperleft ;
  int pa, pb, pc ;
  if (pp > left)
    pa = pp - left ;
  else
    pa = left - pp ;
  if (pp > above)
    pb = pp - above ;
  else
    pb = above - pp ;
  if (pp > upperleft)
    pc = pp - upperleft ;
  else
    pc = upperleft - pp ;

  if (pa <= pb && pa <= pc)
    return left ;
  else if (pb <= pc)
    return above ;
  else
    return upperleft ;
}


inline UBYTE4 ChunkType (const BYTE1 type [5]) 
{
  UBYTE4 value = type [0] | (type [1] << 8) 
               | (type [2] << 16) | (type [3] << 24) ;
  return value ;
}

}

#endif
