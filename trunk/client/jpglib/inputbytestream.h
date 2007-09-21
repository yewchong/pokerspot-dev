#ifndef CB__INPUTBYTESTREAM_H
#define CB__INPUTBYTESTREAM_H
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

#include <stdexcept>
#include "systemspecific.h"

namespace Colosseum
{
//
//  Title:  InputByteStream definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class defines byte level input used by image decoders.
//

class InputByteStream
{
public:
  InputByteStream () ;
  virtual ~InputByteStream () {}

  UBYTE1 getByte () ;
  UBYTE2 getBigEndianWord () ;
  UBYTE4 getBigEndianLong () ;

  int read (char *, int) ;
  bool moreData () ;

  void enterBitMode (int initialposition) ;

#if defined (_MSC_VER)
  typedef std::fpos<int> POSITIONTYPE ;
#else
  typedef std::char_traits<char>::pos_type POSITIONTYPE ;
#endif

  virtual POSITIONTYPE tellg () { return 0 ; }
  virtual void seekg (POSITIONTYPE) {}

  class StreamError ;

protected:
  virtual void fillBuffer () = 0 ;
  virtual bool endReached () = 0 ;

  UBYTE1 *current_byte ;
  UBYTE1 *buffer_limit ;
  int bit_position ;
} ;


class InputByteStream::StreamError : public std::logic_error 
{
public:
  StreamError (const std::string &msg) : std::logic_error (msg) {} 
} ;

inline bool InputByteStream::moreData ()
{
  bool result = (current_byte < buffer_limit) || ! endReached () ;
  return result ;
}

inline void InputByteStream::enterBitMode (int initialposition)
{
  if (bit_position >= 0)
    return ; // Already in bit mode
    
  bit_position = initialposition ;
  return ;
}

} // End Namespace 

#endif
