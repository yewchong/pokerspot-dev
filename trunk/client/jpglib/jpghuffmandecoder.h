#ifndef CB__JPDEHUFF_H
#define CB__JPDEHUFF_H
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

//
//  Title:  JPEG Definitions and Utility Functions
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//


//
//  Title:  JPEG Decoder Huffman Table Class Implementation
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class represents a Huffman Table used by the JpegDecoder
//    class.
//


#include "jpgpvt.h"
#include "bitmapimage.h"

namespace Colosseum
{
class JpegInputStream ;
}
namespace ColosseumPrivate
{
class JpegHuffmanDecoder
{
public:
  JpegHuffmanDecoder () ;
  virtual ~JpegHuffmanDecoder () {}

  // DECODING FUNCTIONS

  // Returns true if the table has been defined...in other words,
  // if ReadTable () has completed successfully. This function is
  // called before the table is used to decode a scan to ensure
  // the the image does not reference a Huffman Table that has
  // not been defined.
  bool tableDefined () const ;

  // This function reads a Huffman table from the input stream.
  unsigned int readTable (Colosseum::JpegInputStream &) ;

  // Function to decode the next value in the input stream.
  int decode (Colosseum::JpegInputStream &) ;

  // This is a debugging function that writes the Huffman table
  // to a stream.
  void printOn (std::ostream &strm) const ;

private:
  JpegHuffmanDecoder (const JpegHuffmanDecoder &) ;
  JpegHuffmanDecoder &operator=(const JpegHuffmanDecoder &) ;

  // This function builds the structures needed for Huffman
  // decoding after the table data has been read.
  void makeTable (UBYTE1 huffbits [JPEGMAXHUFFMANCODELENGTH]) ;

  // Maximum Huffman code value of length N
  int maxcode [JPEGMAXHUFFMANCODELENGTH] ;
  // Minimum Huffman code value of length N
  int mincode [JPEGMAXHUFFMANCODELENGTH] ;
  // Index into "values" for minimum code of length N
  UBYTE1 valptr [JPEGMAXHUFFMANCODELENGTH] ;
  // Huffman values
  UBYTE1 huff_values [JPEGMAXNUMBEROFHUFFMANCODES] ;

  bool table_defined ;

  unsigned int minimum_code_length ;
} ;

} // End Namespace ColosseumPrivate
#endif
