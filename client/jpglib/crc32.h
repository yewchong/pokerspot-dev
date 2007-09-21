#ifndef CB__MSCRC32_H
#define CB__MSCRC32_H
//
//  Title:  Cyclic Reducancy Check Template Class Definition
//
//  Copyright 1999, Colosseum Builders, Inc.
//  All rights reserved.
//
//  Colosseum Builders, Inc. makes no warranty, expressed or implied
//  with regards to this software. It is provided as is.
//
//  Author:  John M. Miano (miano@colosseumbuilders.com)
//
//  Date:    March 15, 1999
//
//  Version: 1
//
//  Description:
//
//    This is a template class for implementing various forms of the
//    CRC function. (Hand-initialized for MSVC++)
//
//    Imporant Member functions:
//
//    reset - Places the CRC register in its initial state.
//    update - Update the value of the CRC register from a buffer.
//    value - Fetch the value of the CRC register.
//
//  Template Parameters:
//
//    BITCOUNT: The number of bits in the CRC (16 or 32)
//    POLYNOMIAL: The CRC polynomial value
//    REVERSED: true => Use the reverse form of the polynomial
//              false => The non-reversed form.
//    INITIAL:  The initial value for the CRC Register
//    FINALMASK: A value to be XORed with the register when
//               retrieving the CRC value. This value should be zero
//               if no XOR is required and 0xFFFFFFFF otherwise.
//
//  Revision History:
//
//

#include "systemspecific.h"

namespace Colosseum
{

class Crc32
{
public:
  Crc32 () ;
  Crc32 (const Crc32 &) ;
  ~Crc32 () {}
  Crc32 &operator=(const Crc32 &) ;

  UBYTE4 value () const ;
  void reset () ;
  void update (const char *buffer, unsigned int length) ;

private:
  struct CrcTable
  {
    enum { MAXBYTEVALUES = 256 } ;
    CrcTable () ;
    UBYTE4 values [MAXBYTEVALUES] ;
  } ;


  static const CrcTable crc_table ;


  UBYTE4 crc_register ;
} ;


}

#endif
