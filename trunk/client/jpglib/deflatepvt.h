#ifndef CB__DEFLATEPVT_H
#define CB__DEFLATEPVT_H
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


#include <string>

#include "systemspecific.h"

const unsigned int DEFLATEMAXLENGTHLENGTHCODES = 19 ;

const unsigned int DEFLATEMAXLENGTHCODES = 286 ;
const unsigned int DEFLATEMAXDISTANCECODES = 30 ;
const unsigned int DEFLATEMAXNUMBEROFHUFFMANCODES = DEFLATEMAXLENGTHCODES ;
const unsigned int DEFLATEFIRSTLENGTHCODE = 257 ;
const unsigned int DEFLATELENGTHCODECOUNT = DEFLATEMAXLENGTHCODES
                                          - DEFLATEFIRSTLENGTHCODE ;
const unsigned int DEFLATELITERALCOUNT = 256 ;
const unsigned int DEFLATEENDCODE = DEFLATELITERALCOUNT ;

// Maximum Huffman Code Sizes
const unsigned int DEFLATEMAXLENGTHLENGTHCODESIZE = 7 ;
const unsigned int DEFLATEMAXDISTANCECODESIZE = 15 ;
const unsigned int DEFLATEMAXLENGTHCODESIZE = 15 ;

const unsigned int DEFLATEWINDOWSIZE = (1 << 15) ;
const unsigned int DEFLATEWINDOWMASK = DEFLATEWINDOWSIZE - 1 ;

const unsigned int DEFLATELONGESTLENGTH = 258 ;

static const UBYTE1 DEFLATELENGTHORDER [DEFLATEMAXLENGTHLENGTHCODES] =
{
  16, 17, 18, 0,  8, 7,  9, 6, 10, 5,
  11,  4, 12, 3, 13, 2, 14, 1, 15,
} ;

static inline const char *Binary (int vv, int ll)
{
  static const char digit [2] = { '0', '1' } ;
  static char value [33] ;
  value [0] = '\000' ;
  for (unsigned int ii = ll , jj = 0 ; ii > 0 ; -- ii, ++ jj)
  {
    value [jj] = digit [(vv & (1 << (ii-1))) != 0 ] ;
    value [jj+1] = 0 ;
  }
  return value ;
}

#endif
