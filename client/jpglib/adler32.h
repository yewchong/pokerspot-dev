#ifndef CB__ADLER32_H
#define CB__ADLER32_H
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
//  Title:  PNG Checksum Function Declarations
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    Adler32 Checksum Definition and Implementation
//

#include "systemspecific.h"

inline UBYTE4 Adler (UBYTE4 adler, UBYTE1 value)
{
  static const UBYTE4 prime = 65521 ;

  UBYTE4 lo = adler & 0xFFFF ;
  UBYTE4 hi = (adler >> 16) & 0xFFFF ;

  lo = (lo + value) % prime ;
  hi = (lo + hi) % prime ;
  UBYTE4 result = (hi << 16) | lo ;
  return result ;
}

#endif
