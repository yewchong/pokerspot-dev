#ifndef CB__DEFLATEHUFFENCODER_H
#define CB__DEFLATEHUFFENCODER_H

//
//  Title:  Deflate Huffman Encoding Class
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
//    This is a huffman encoding class designed for Deflate compression.
//

#include "array.h"

namespace ColosseumPrivate
{

class DeflateHuffmanEncoder
{
public:
  DeflateHuffmanEncoder (unsigned int valuecount=256) ;
  virtual ~DeflateHuffmanEncoder () {}

  // ENCODING FUNCTIONS

  // This function resets the table so that the object can be used
  // over again.
  void reset () ;

  // This function increases the frequency for a huffman value.
  void incrementFrequency (unsigned int value) ;

  // This function creates the Huffman codes from the code frequencies.
  void buildTable (unsigned int maxlength) ;

  // This function returns the Huffman code and code length to encode the
  // specified value.
  void encode (unsigned int value, unsigned int &code, unsigned int &size) const ;

private:
  DeflateHuffmanEncoder (const DeflateHuffmanEncoder &) ;
  DeflateHuffmanEncoder &operator=(const DeflateHuffmanEncoder &) ;

  // This function builds the structures needed for Huffman
  // decoding after the table data has been read.
  void makeTable () ;

  // frequencies [n] is the number of times the value "n" needs to
  // be encoded.
  Array<unsigned int> frequencies ;

  // Values used to encode values.
  //   ehufsi [n] is the number of bits required to code "n"
  //   ehufco [n] is the Huffman code for "n"
  Array<unsigned int> ehufsi ;
  Array<unsigned int> ehufco ;

  unsigned int value_count ;
} ;

} // End Namespace ColosseumPrivate

#endif
