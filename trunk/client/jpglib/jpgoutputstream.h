#ifndef CB__JPGOUTPUTSTREAM_H
#define CB__JPGOUTPUTSTREAM_H
//
//  Title:  JpegOutputFileStream Class Definition
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
//    The JpegOutputStream class defines the attributes required for JPEG
//    output.
//

#include <climits>
#include "checks.h"
#include "outputbytestream.h"

namespace Colosseum
{

class JpegOutputStream : virtual public OutputByteStream
{
public:
  void outputBits (int bits, unsigned int count) ;
  void exitBitMode () ;
} ;

//
//  Description:
//
//    This function outputs a single bit to the output stream.
//
//    Individual its are buffered using the bit_buffer and bit_count member
//    variables.
//
//    This function throws an exception if it is called with a zero bit count.
//    We use this for error trapping since no place in the rest of the
//    code should attempt to write zero bits.
//
//  Parameters:
//    bits: The bit string to be output
//    count: The number of bits to output
//
inline void JpegOutputStream::outputBits (int bits, unsigned int count)
{
  CHECKARGUMENT (count != 0 && count <= 16) ;
  static const int masks [17] = { 0, 1, 1<<1, 1<<2, 1<<3, 1<<4, 1<<5, 1<<6, 1<<7, 
                                   1<<8, 1<<9, 1<<10, 1<<11, 1<<12, 1<<13, 1<<14, 1<<15 } ;

  for (unsigned int ii = count ; ii > 0 ; -- ii)
  {
    if ((bits & masks [ii]) != 0)
      *current_byte  |= masks [bit_position] ;
    -- bit_position ;
    if (bit_position == 0)
    {
      if (*current_byte == 0xFF)
      {
        // B.1.1.5 In order to ensure that a marker does not occur within
        // compressed data, the 2-byte sequence FF00 is used to represent
        // the 1-byte FF.
        nextByte () ;
        *current_byte = 0x00 ;      
      }
      nextByte () ;
      bit_position = CHAR_BIT ;
    }
  }
  return ;
}


inline void JpegOutputStream::exitBitMode ()
{
  if (bit_position != CHAR_BIT)
    nextByte () ;
  bit_position = -1 ;
  return ;
}


}
#endif
