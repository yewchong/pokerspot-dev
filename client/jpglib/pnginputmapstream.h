#ifndef CB__PNGINPUTMAPSTREAM_H
#define CB__PNGINPUTMAPSTREAM_H

//
//  Title:  PngInputMapStream class definition
//
//  Copyright 1999, Colosseum Builders, Inc.
//  All rights reserved.
//
//  Colosseum Builders, Inc. makes no warranty, expressed or implied
//  with regards to this software. It is provided as is.
//
//  Author:  John M. Miano (miano@colosseumbuilders.com)
//
//  Date:    April 25, 1999
//
//  Version: 1
//
//  Description:
//
//    This class defines a PNG input stream that uses a Win32 file mapping.
//
//  Revision History:
//
//

#include <windows.h>

#include <fstream>
#include "pnginputstream.h"

namespace Colosseum
{

class PngInputMapStream : virtual public PngInputStream
{
public:
  PngInputMapStream () ;
  virtual ~PngInputMapStream () ;

  virtual UBYTE4 readChunkLength () ;
  virtual void readChunkData (UBYTE4 length, UBYTE1 *&buffer) ;
  virtual UBYTE4 readCrc () ;
  virtual bool endReached () ;
  virtual void readRaw (UBYTE1 *&buffer, unsigned int count) ;

  operator bool () ;
  void open (const std::string &filename) ;
  void close () ;

private:
  PngInputMapStream (const PngInputMapStream &) ;
  PngInputMapStream &operator=(const PngInputMapStream &) ;

  HFILE file_handle ;     // Handle for the open file
  HANDLE file_mapping ;   // Handle for the mapping
  UBYTE1 *file_pointer ;
  UBYTE1 *file_limit ;
  void *map_location ;  // Virtual address where the file is mapped

  bool is_open ;
} ;



}



#endif
