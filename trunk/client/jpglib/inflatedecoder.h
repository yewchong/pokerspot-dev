#ifndef CB__INFLATEDECODER_H
#define CB__INFLATEDECODER_H
//
// Copyright (c) 1997,1998,1999 Colosseum Builders, Inc.
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
//  Title:  PNG Decoder class definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//
#include <iostream>
#include <stdexcept>

#include "systemspecific.h"

namespace Colosseum
{

class InflateInputStream ;
class InflateHuffmanDecoder ;

class InflateDecoder 
{
public:
  InflateDecoder () ;
  virtual ~InflateDecoder () ;
  void setVerbose (bool) ;
  bool getVerbose () const ;
  void openStream (InflateInputStream &) ;
  int decode (InflateInputStream &strm, unsigned int count, UBYTE1 output []) ;

  class StreamError ;

private:
  InflateDecoder (const InflateDecoder &) ;
  InflateDecoder &operator=(InflateDecoder &) ;

  // Deflate Functions
  void startNewDataSet (InflateInputStream &) ;
  void checkAdler (InflateInputStream &) ;
  void readLengths (InflateInputStream &strm,
                    InflateHuffmanDecoder &,
                    unsigned int [],
                    unsigned int) ;
  void freeData () ;
  // Deflate State Variables
  UBYTE4 stream_adler ;       // Adler32 Checksum value
  UBYTE1 *lz_window ;         // LZ77 Sliding Window
  unsigned int window_position ;  // Output position in the LZ77 window  
  unsigned int copy_position ;    // Input posiiton in the LZ77 window
  unsigned int copy_count ;   // Number bytes remaining from a copy operation
  InflateHuffmanDecoder *literal_table ;
  InflateHuffmanDecoder *distance_table ;
  bool final_data_set ;  // Set when Final data set is read
  bool no_more_data ;    // Set when no more data is available.
  UBYTE2 literal_count ; // Number remaining literal bytes

  bool verbose_flag ;
} ;

inline void InflateDecoder::setVerbose (bool value)
{
  verbose_flag = value ;
  return ;
}

inline bool InflateDecoder::getVerbose () const
{
  return verbose_flag ;
}


class InflateDecoder::StreamError : public std::exception
{
private:
  std::string message ;
public:
  StreamError (const std::string &msg) : message (msg) {} 
  StreamError (const StreamError &source) : message (source.message) {}
  StreamError &operator=(const StreamError &source)
  {
    if (this != &source)
    {
      message = source.message ;
    }
    return *this ;
  }
  virtual const char *what () const throw () { return message.c_str () ; } 
} ;


} // End Namespace

#endif
