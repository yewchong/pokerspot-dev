#ifndef CB__INFLATEHUFFMANDECODER_H
#define CB__INFLATEHUFFMANDECODER_H
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
//  Title:  PngHuffmanDecoder definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class handles Huffman decoding for PNG images.
//

#include <iostream>
#include <vector>
#include "inflateinputstream.h"
#include "checks.h"

namespace Colosseum
{

class InflateHuffmanDecoder
{
public:
  InflateHuffmanDecoder (){}
  virtual ~InflateHuffmanDecoder () {}

  // DECODING FUNCTIONS
  void makeTable (unsigned int maxcodesize,
                  unsigned int lengthcount,
                  const unsigned int huffbits []) ;

  // This is a debugging function that writes the Huffman table
  // to cout.
  void dump () const ;

  int decode (InflateInputStream &) const ;

private:
  InflateHuffmanDecoder (const InflateHuffmanDecoder &) ;
  InflateHuffmanDecoder &operator=(const InflateHuffmanDecoder &) ;

  // Maximum Huffman code value of length N
  std::vector<int> maxcode  ;
  // Minimum Huffman code value of length N
  std::vector<int> mincode ;
  // Index into "values" for minimum code of length N
  std::vector<unsigned int> valptr  ;          
  // Huffman values
  std::vector<unsigned int> huff_values ;

  unsigned int value_count ;
  unsigned int max_code_size ;
} ;

//#define INLINEHUFFMAN
#if defined (INLINEHUFFMAN) 
inline int InflateHuffmanDecoder::decode (InflateInputBitStream &inputstream) const
{
  // We can't cheat by reading multiple bits here because read are reversing the
  // bits as they appear in the input stream.

  int code = inputstream.nextBit () ;
  unsigned int codelength ; // Code length minus 1

  // Here we are taking advantage of the fact that 1 bits are used as
  // a prefix to the longer codes.
  for (codelength = 0 ;
      (code > maxcode [codelength]) ;
      ++ codelength)
  {
    ASSERT (codelength < max_code_size)
    code = ((code << 1) | inputstream.nextBit ()) ;
  }
  if (codelngth >= max_code_size)
    throw InflateInputBitStream::StreamError("Invalid Huffman Code") ;

  // Now we have a Huffman code of length (codelength + 1) that
  // is somewhere in the range
  // mincode [codelength]..maxcode [codelength].

  // This code is the (offset + 1)'th code of (codelength + 1) ;
  int offset = code - mincode [codelength] ;

  // valptr [codelength] is the first code of length (codelength + 1)
  // so now we can look up the value for the Huffman code in the table.
  int index = valptr [codelength] + offset ;
  unsigned int value = huff_values [index] ;
  return value ;
}
#endif


} // End Namespace 

#endif
