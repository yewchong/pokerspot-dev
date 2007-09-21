#ifndef CB__PNGINPUTSTREAM_H
#define CB__PNGINPUTSTREAM_H
//
//  Title:  PNG Input File Stream
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
//    This class is a PNG input stream attached to a file.
//

#include "inflateinputstream.h"

namespace Colosseum
{

class PngInputStream : virtual public InflateInputStream
{
public:
  bool getNextChunk () ;
  const UBYTE1 *getChunkType () const ;
  UBYTE4 getChunkDataLength () const ;
  const UBYTE1 *getChunkData () const ;
  virtual void readRaw (unsigned int count, UBYTE1 *&buffer) = 0 ;

protected:
  virtual void fillBuffer () ;

  virtual UBYTE4 readChunkLength () = 0 ;
  virtual void readChunkData (UBYTE4 length, UBYTE1 *&buffer) = 0 ;
  virtual UBYTE4 readCrc () = 0 ;

  UBYTE1 chunk_type [4] ;
  UBYTE1 *chunk_data ;
  UBYTE4 chunk_length ;
} ;

inline const UBYTE1 *PngInputStream::getChunkType () const
{
  return chunk_type ;
}

inline UBYTE4 PngInputStream::getChunkDataLength () const
{
  return chunk_length ;
}

inline const UBYTE1 *PngInputStream::getChunkData () const
{
  return chunk_data ;
}

}

#endif
