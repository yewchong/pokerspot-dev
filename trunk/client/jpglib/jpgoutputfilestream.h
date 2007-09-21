#ifndef CB__JPGOUTPUTFILESTREAM_H
#define CB__JPGOUTPUTFILESTREAM_H
//
//  Title:  JpegOutputFileStream Class
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
//    The JpegOutputFileStream handles JPEG output usig a file.
//

#include <fstream>
#include "jpgoutputstream.h"

namespace Colosseum
{

class JpegOutputFileStream : virtual public JpegOutputStream
{
public:
  JpegOutputFileStream () ;
  virtual ~JpegOutputFileStream () ;

  void open (const std::string &) ;
  void close () ;
  operator bool () ;

protected:
  virtual void flushBuffer () ;

private:
  JpegOutputFileStream (const JpegOutputFileStream &) ;
  JpegOutputFileStream &operator=(const JpegOutputFileStream &) ;

  std::ofstream output_stream ;
  bool is_open ;
  UBYTE1 *output_buffer ;
  unsigned int output_buffer_size ;
} ;


}


#endif
