#ifndef CB__CRC_H
#define CB__CRC_H
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
//    CRC function.
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

template <unsigned int BITCOUNT,
          unsigned long POLYNOMIAL,
          bool REVERSE,
          unsigned long INITIAL,
          unsigned long FINALMASK>
class Crc
{
public:
  Crc () ;
  Crc (const Crc &) ;
  ~Crc () {}
  Crc &operator=(const Crc &) ;

  UBYTE4 value () const ;
  void reset () ;
  void update (const char *buffer, unsigned int length) ;

  struct CrcTable
  {
    enum { MAXBYTEVALUES = 256 } ;
    CrcTable () ;
    UBYTE4 values [MAXBYTEVALUES] ;
  } ;
private:


  static const CrcTable crc_table ;


  UBYTE4 crc_register ;
} ;


}

#endif
