#ifndef CB__JPGINPUTFILESTREAM_H
#define CB__JPGINPUTFILESTREAM_H

#include <fstream>

#include "jpginputstream.h"

namespace Colosseum
{

class JpegInputFileStream : virtual public JpegInputStream
{
public:
  JpegInputFileStream (unsigned int buffersize = 4096) ;
  virtual ~JpegInputFileStream () ;

  operator bool () ;
  void open (const std::string &filename) ;
  void close () ;
  
  virtual POSITIONTYPE tellg () ;
  virtual void seekg (POSITIONTYPE) ;

private:
  JpegInputFileStream (const JpegInputFileStream &) ;
  JpegInputFileStream &operator=(const JpegInputFileStream &) ;

  virtual void fillBuffer () ;
  virtual bool endReached () ;

  unsigned int buffer_size ;
  UBYTE1 *input_buffer ;
  std::ifstream input_stream ;
} ;

}



#endif
