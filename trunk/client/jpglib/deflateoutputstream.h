#ifndef CB__DEFALTEOUTPUTSTREAM_H
#define CB__DEFALTEOUTPUTSTREAM_H
//
//  Title:  Deflate Output Stream Class
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
//    This file defines an abstract class that represents
//    an output stream suitable for deflate compression.
//

#include <climits>
#include "outputbytestream.h"
#include "checks.h"

namespace Colosseum
{

class DeflateOutputStream : virtual public OutputByteStream
{
public:                      
  DeflateOutputStream () {}   
  virtual ~DeflateOutputStream () {}
  void writeBits (int bits, int count) ;
  void exitBitMode () ;

private:
  DeflateOutputStream (const DeflateOutputStream &) ;
  DeflateOutputStream &operator=(const DeflateOutputStream &) ;
} ;

inline void DeflateOutputStream::writeBits (int bits, int count)
{
  ASSERT (count <= 16) ;
  ASSERT (bit_position >= 0 && bit_position <= CHAR_BIT) ;

  static const UBYTE4 masks [32] =
  {
    1,     1<<1,  1<<2,  1<<3,  1<<4,  1<<5,  1<<6,  1<<7,
    1<<8,  1<<9,  1<<10, 1<<11, 1<<12, 1<<13, 1<<14, 1<<15,
    1<<16, 1<<17, 1<<18, 1<<19, 1<<20, 1<<21, 1<<22, 1<<23,
    1<<24, 1<<25, 1<<26, 1<<27, 1<<28, 1<<29, 1<<30, 1<<31,
  } ;
  static const UBYTE4 complements [8] =
  {
    ~1,     ~(1<<1),  ~(1<<2),  ~(1<<3),  ~(1<<4),  ~(1<<5),  ~(1<<6),  ~(1<<7),
  } ;
  for (unsigned int ii = 0 ; ii < count ; ++ ii, ++ bit_position)
  {
    if (bit_position >= CHAR_BIT)
    {
      nextByte () ;
      bit_position = 0 ;
    }
    if ((bits & masks [ii]) != 0)
      *current_byte |= masks [bit_position] ;
    else
      *current_byte &= complements [bit_position] ;
  }
  return ;
}

inline void DeflateOutputStream::exitBitMode ()
{
  if (bit_position != 0)
    nextByte () ;
  bit_position = -1 ;
  return ;
}




}

#endif
