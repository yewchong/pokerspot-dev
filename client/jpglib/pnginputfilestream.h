#ifndef CB__PNGINPUTFILESTREAM_H
#define CB__PNGINPUTFILESTREAM_H
//
//  Title:  PngInputFileStream class Definition
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
//    The PngInputFileStream file stream implements PNG output 
//    using a file.
//

#include <fstream>
#include "pnginputstream.h"

namespace Colosseum
{

class PngInputFileStream : virtual public PngInputStream
{
public:
  PngInputFileStream (unsigned int buffersize = 4096) ;
  virtual ~PngInputFileStream () ;

  virtual UBYTE4 readChunkLength () ;
  virtual void readChunkData (UBYTE4 length, UBYTE1 *&buffer) ;
  virtual UBYTE4 readCrc () ;
  virtual bool endReached () ;
  virtual void readRaw (unsigned int count, UBYTE1 *&buffer) ;

  operator bool () ;
  void open (const std::string &filename) ;
  void close () ;

private:
  PngInputFileStream (const PngInputFileStream &) ;
  PngInputFileStream &operator=(const PngInputFileStream &) ;

  unsigned int buffer_size ;
  UBYTE1 *input_buffer ;
  std::ifstream input_stream ;
} ;

}



#endif
