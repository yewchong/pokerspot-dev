#ifndef CB__JPGINPUTMAPSTREAM_H
#define CB__JPGINPUTMAPSTREAM_H
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

#include <fstream>
#include <windows.h>

#include "jpginputstream.h"

namespace Colosseum
{

class JpegInputMapStream : virtual public JpegInputStream
{
public:
  JpegInputMapStream () ;
  virtual ~JpegInputMapStream () ;

  operator bool () ;
  void open (const std::string &filename) ;
  void close () ;
  
  virtual POSITIONTYPE tellg () ;
  virtual void seekg (POSITIONTYPE) ;

private:
  JpegInputMapStream (const JpegInputMapStream &) ;
  JpegInputMapStream &operator=(const JpegInputMapStream &) ;

  virtual void fillBuffer () ;
  virtual bool endReached () ;

  HFILE file_handle ;
  HANDLE file_mapping ;
  void *map_location ;

  bool is_open ;
} ;

}



#endif
