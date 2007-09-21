#ifndef CB__JPGINPUTSTREAM_H
#define CB__JPGINPUTSTREAM_H
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

#include "inputbytestream.h"

namespace Colosseum
{

class JpegInputStream : virtual public InputByteStream
{
public:
  int nextBit () ;
  int getBits (unsigned int count) ;
  void exitBitMode () ;
} ;


inline int JpegInputStream::nextBit ()
{
  if (bit_position == 0)
  {
    if (*current_byte == 0xFF)
    {

      ++ current_byte ;
      if (current_byte >= buffer_limit)
      {
        fillBuffer () ;
        if (! moreData ())
          throw StreamError ("Premature end of file") ;
      }
      if (*current_byte != 0)
        throw StreamError ("Invalid Marker in Compressed Data") ;
    }
    ++ current_byte ;
    bit_position = CHAR_BIT ;
    if (current_byte >= buffer_limit)
    {
      fillBuffer () ;
      if (! moreData ())
        throw StreamError ("Premature end of stream") ;
    }
  }

  // Extract the bit value
  -- bit_position ;
  int result ;
  static const unsigned int masks[] = { 1U, 2U, 4U, 8U, 16U, 32U, 64U, 128U, } ;
  if ((masks [bit_position] & *current_byte) != 0)
    result = 1 ;
  else
    result = 0 ;
  return result ;
}


inline int JpegInputStream::getBits (unsigned int count) 
{
  static const unsigned int masks[] = { 0, 1U, 3U, 7U, 15U, 31U, 63U, 127U, 255U, } ;
  int result ;
  if (count <= bit_position)
  {
    // Easy case. We have already enough bits to process the request.
    bit_position -= count ;
    result = (*current_byte >> bit_position) & masks [count] ;
  }
  else
  {
    result = 0 ;
    // Save the remaining bits from the current byte.
    if (bit_position > 0)
    {
      count -= bit_position ;
      result = (*current_byte & masks [bit_position]) << count ;
    }

    while (count > 0)
    {
      // Move to the next byte.
      if (*current_byte == 0xFF)
      {
        ++ current_byte ;
        if (current_byte >= buffer_limit)
        {
          fillBuffer () ;
          if (! moreData ())
            throw StreamError ("Premature end of file") ;
        }
        if (*current_byte != 0)
          throw StreamError ("Invalid Marker in Compressed Stream") ;
      }
      ++ current_byte ;
      
      bit_position = CHAR_BIT ;
      if (current_byte >= buffer_limit)
      {
        fillBuffer () ;
        if (! moreData ())
          throw StreamError ("Premature end of file") ;
      } 
      // Process markers.

      if (count >= CHAR_BIT)
      {
        // We need the all of the new byte for the request.
        count -= CHAR_BIT ;
        result |= (*current_byte << count) ;
        bit_position = 0 ;
      }
      else
      {
        // We only need part of the new byte. 
        static const unsigned int highmask [] = 
          { 0, 128U, 192U, 224U, 240U, 248U, 252U, 254U, 255U, } ;
        result |= (*current_byte & highmask [count]) 
                  >> ( CHAR_BIT - count) ;
        bit_position -= count ;
        break ; // All Done
      }
    }
  }
  return result ;
}

inline void JpegInputStream::exitBitMode () 
{
  if (bit_position >= 0)
  {
    // The FF00 sequence represents the bit value FF. If we are in the
    // middle of this sequence we need to skip two bytes.
    if (*current_byte == 0xFF)
    {
      ++ current_byte ;
      if (current_byte >= buffer_limit)
      {
        fillBuffer () ;
        if (! moreData ())
          throw StreamError ("Premature end of file") ;
      }
      if (*current_byte != 0)
        throw StreamError ("Invalid Marker in Compressed Stream") ;
      ++ current_byte ;
    }
    else
    {
      ++ current_byte ;
    }
    bit_position = -1 ;
  }
  return ;
}

} 


#endif
