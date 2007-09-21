#ifndef CB__INFLATEINPUTSTREAM_H
#define CB__INFLATEINPUTSTREAM_H
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

#include <climits>
#include "inputbytestream.h"

namespace Colosseum
{
//
//  Title:  InflateInputStream definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class defines input required by the Inflate decoders.
//

class InflateInputStream : virtual public InputByteStream
{
public:
  int nextBit () ;
  int getBits (unsigned int count) ;
  void exitBitMode () ;
} ;

//
//  Description:
//
//    Retrieves the next bit in the input stream. The bits are extracted low to high.
//
//  Return Value:
//
//    The next bit in the input stream.
//
inline int InflateInputStream::nextBit ()
{
  if (bit_position >= CHAR_BIT)
  {
    ++ current_byte ;
    bit_position = 0 ;
    if (current_byte >= buffer_limit)
    {
      fillBuffer () ;
      if (! moreData ())
        throw StreamError ("Premature end of stream") ;
    }
  }
  // Extract the bit value
  int result ;
  static const unsigned int masks[] = { 1U, 2U, 4U, 8U, 16U, 32U, 64U, 128U, } ;
  if ((masks [bit_position] & *current_byte) != 0)
    result = 1 ;
  else
    result = 0 ;
  ++ bit_position ;
  return result ;
}

//
//  Description:
//
//    This function extracts a number of bits from the input stream. The bits
//    are extracted from low to high.
//
//  Parameter:
//
//    count: The number of bits to read.
//
//  Return Value:
//
//    'count' bits from the input stream.
//
inline int InflateInputStream::getBits (unsigned int count) 
{
  static const unsigned int masks[] = { 0, 1U, 3U, 7U, 15U, 31U, 63U, 127U, 255U, } ;
  int result ;
  if (count <= CHAR_BIT - bit_position)
  {
    // Easy case. We have already enough bits to process the request.
    result = (*current_byte >> bit_position) & masks [count] ;
    bit_position += count ;
  }
  else
  {
    result = 0 ;
    int bitswritten = 0 ;
    // Save the remaining bits from the current byte.
    if (bit_position < CHAR_BIT)
    {
      bitswritten = CHAR_BIT - bit_position ;
      result = (*current_byte >> bit_position)  & masks [bitswritten] ;
      count -= bitswritten ;
    }

    while (count > 0)
    {
      // Move to the next byte.
      ++ current_byte ;
      bit_position = 0 ;
      if (current_byte >= buffer_limit)
      {
        fillBuffer () ;
        if (! moreData ())
          throw StreamError ("Premature end of file") ;
      }

      if (count > CHAR_BIT)
      {
        // We need the all of the new byte for the request.
        count -= CHAR_BIT ;
        result |= (*current_byte << bitswritten) ;
        bit_position = 0 ;
        bitswritten += CHAR_BIT ;
      }
      else
      {
        result |= (*current_byte & masks [count]) << bitswritten ;
        bit_position += count ;
        break ; // All Done
      }
    }
  }
  return result ;
}

inline void InflateInputStream::exitBitMode ()
{
  if (bit_position >= 0)
    ++ current_byte ;
  bit_position = -1 ;
  return ;
}

} // End Namespace

#endif
