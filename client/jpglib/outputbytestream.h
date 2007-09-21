#ifndef CB__OUTPUTBYTESTREAM_H
#define CB__OUTPUTBYTESTREAM_H
//
//  Title:  Output Byte Stream Class
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
//    This is an abstract class the represents an output stream of bytes.
//

#include <stdexcept>
#include "systemspecific.h"

namespace Colosseum
{

class OutputByteStream
{
public:
  OutputByteStream () ;
  virtual ~OutputByteStream () {} ;

  void write (const char *, int) ;
  void writeByte (UBYTE1) ;
  void writeBigEndianWord (UBYTE2) ;
  void writeBigEndianLong (UBYTE4) ;

  void enterBitMode (int initialposition) ;
  void nextByte () ;

  unsigned int remainingBufferSpace () const ;
  class StreamError ;
protected:
  virtual void flushBuffer () = 0 ;
  UBYTE1 *current_byte ; // Current Byte Position
  UBYTE1 *buffer_limit ; // Limit of the output byffer
  int bit_position ;

} ;

inline void OutputByteStream::nextByte ()
{
  ++ current_byte ;
  if (current_byte >= buffer_limit)
    flushBuffer () ;
  *current_byte = 0 ;
  return ;
}

class OutputByteStream::StreamError : public std::exception
{
private:
  std::string message ;
public:
  StreamError (const std::string &msg) : message (msg) {}
  StreamError (const StreamError &source) : message (source.message) {}
  virtual ~StreamError () throw () {}
  StreamError &operator=(const StreamError &source)
  {
    if (&source != this)
    {
      message = source.message ;
    }
    return *this ;
  }
  const char *what () const throw () { return message.c_str () ; } 
} ;

}

#endif
