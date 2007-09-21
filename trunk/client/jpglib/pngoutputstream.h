#ifndef CB__PNGOUTPUTSTREAM_H
#define CB__PNGOUTPUTSTREAM_H
//
//  Title:  PNG Output Stream
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
//    This an abstract class the represents an PNG output stream.
//

#include "deflateoutputstream.h"


namespace Colosseum
{

class PngOutputStream : virtual public DeflateOutputStream 
{
public:
  PngOutputStream () ;
  virtual ~PngOutputStream () {}
  void startChunk (char chunktype [4]) ;
  void endChunk () ;
  virtual void writeRaw (const char* buffer, unsigned int size) = 0 ;
  void exitBitMode () ;

protected:
  char chunk_type [4] ;

private:
  PngOutputStream (const PngOutputStream &) ;
  PngOutputStream &operator=(const PngOutputStream &) ;

} ;

} // End Namespace Colosseum


#endif
