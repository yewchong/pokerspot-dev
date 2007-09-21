#ifndef CB__JPGHUFFMANENCODER_H
#define CB__JPGHUFFMANENCODER_H
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
//  Title:  JpegEncoderHuffmanTable class definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class represents a Huffman Table used for compression
//    by the JpegEncoder class.
//

#include "jpgpvt.h"
#include "checks.h"

namespace Colosseum
{
class JpegOutputStream ;
}
namespace ColosseumPrivate
{

class JpegHuffmanEncoder
{
public:
  JpegHuffmanEncoder () ;
  virtual ~JpegHuffmanEncoder () {}

  // ENCODING FUNCTIONS

  // This function resets the table so that the object can be used
  // over again.
  void reset () ;

  // This function increases the frequency for a huffman value.
  void incrementFrequency (unsigned int value) ;

  // This function creates the Huffman codes from the code frequencies.
  void buildTable () ;

  // This function returns the Huffman code and code length to encode the
  // specified value.
  void encode (unsigned int value, UBYTE2 &code, UBYTE1 &size) const ;

  // Returns the number of bytes required to write the table to the output
  // file.
  unsigned int outputSize () const ;

  // Function to print the table definition to the output stream.
  void printTable (Colosseum::JpegOutputStream &outputstream) const ;

private:
  JpegHuffmanEncoder (const JpegHuffmanEncoder &) ;
  JpegHuffmanEncoder &operator=(const JpegHuffmanEncoder &) ;

  // This function builds the structures needed for Huffman
  // decoding after the table data has been read.
  void MakeTable () ;

  // frequencies [n] is the number of times the value "n" needs to
  // be encoded.
  unsigned int frequencies [JPEGMAXNUMBEROFHUFFMANCODES + 1] ;

  // Values used to represent Huffman tables in a JPEG stream
  //  huff_bits [n] is the number of codes of length "n+1"
  //  huff_values is the list of Huffman values sorted in order
  //   of code length.
  UBYTE1 huff_bits [2 * JPEGMAXHUFFMANCODELENGTH] ; // 2x needed for encoding only.
  UBYTE1 huff_values [JPEGMAXNUMBEROFHUFFMANCODES] ;

  // Values used to encode values.
  //   ehufsi [n] is the number of bits required to code "n"
  //   ehufco [n] is the Huffman code for "n"
  UBYTE1 ehufsi [JPEGMAXNUMBEROFHUFFMANCODES+ 1] ;
  UBYTE2 ehufco [JPEGMAXNUMBEROFHUFFMANCODES+1] ;

  // The flag is set to true when the Huffman code sizes has been determined.
  // It is cleared when the object is Reset().
  bool sizes_found ;
} ;

//
//  Description:
//
//    Function to increment the frequency for a value.
//
//  Parameters:
//    value:  The value whose frequency is to be incremented
//

inline void JpegHuffmanEncoder::incrementFrequency (unsigned int value)
{
  // Once the Huffman codes have been generated for this object, the reset()
  // function must be called before we can gather data again.
  ASSERT (! sizes_found) ;
  ASSERT (value < JPEGMAXNUMBEROFHUFFMANCODES) ;

  ++ frequencies [value] ;
  return ;
}

//
//  Description:
//
//    This function returns the Huffman Code and Code Size for a given value.
//
//  Parameters:
//    value:  The value to encode
//    code:   The Huffman Code
//    size:   The Huffman Code Length
//
inline void JpegHuffmanEncoder::encode (unsigned int value,
                                      UBYTE2 &code,
                                      UBYTE1 &size) const
{
  ASSERT (value < JPEGMAXNUMBEROFHUFFMANCODES) ; // Make sure value is in range
  ASSERT (ehufsi [value] != 0) ; // Check for missing huffman code

  code = ehufco [value] ;
  size = ehufsi [value] ;
  return ;
}


} // End Namespace ColosseumPrivate

#endif
