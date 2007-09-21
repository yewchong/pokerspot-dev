#ifndef CB__PNGOUTPUTFILESTREAM_H
#define CB__PNGOUTPUTFILESTREAM_H
//
//  Title:  PNG Output File Stream
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
//    This class is a PNG output stream directed to a file.
//

#include <fstream>
#include "pngoutputstream.h"

namespace Colosseum
{

class PngOutputFileStream : virtual public PngOutputStream
{
public:
  PngOutputFileStream () ;
  virtual ~PngOutputFileStream () ;

  void open (const std::string &) ;
  void close () ;
  operator bool () ;
  virtual void writeRaw (const char* buffer, unsigned int size) ;

protected:
  virtual void flushBuffer () ;

private:
  PngOutputFileStream (const PngOutputFileStream &) ;
  PngOutputFileStream &operator=(const PngOutputFileStream &) ;

  std::ofstream output_stream ;
  unsigned int buffer_size ;
  UBYTE1 *output_buffer ;
} ;


}

#endif
