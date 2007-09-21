#ifndef CB__JPENQUAN_H
#define CB__JPENQUAN_H
//
// Copyright (c) 1997,1998 Colosseum Builders, Inc.
// All rights reserved.
//
// Colosseum Builders, Inc. makes no warranty, expressed or implied
// with regards to this software. It is provided as is.
//
// See the README.TXT file that came with this software for restrictions
// on the use and redistribution of this file or send E-mail to
// info@colosseumbuilders.com
//

//
//  Title:  JpegEncoderQuantizationTable class definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class represents quantization table in the JPEG encoder.
//

#include "jpgpvt.h"

namespace Colosseum
{
class JpegDecoder ;
}

namespace ColosseumPrivate
{

class JpegEncoderQuantizationTable
{
public:
  JpegEncoderQuantizationTable() ;
  ~JpegEncoderQuantizationTable() {}

  UBYTE2 operator[](unsigned int index) const ;
  UBYTE2 &operator[](unsigned int index) ;

  void buildScaledTables () ;

  bool isUsed () const ;
  void isUsed (bool) ;
private:
  JpegEncoderQuantizationTable (const JpegEncoderQuantizationTable&) ;
  JpegEncoderQuantizationTable &operator=(const JpegEncoderQuantizationTable&) ;

  // The integer quantization values stored in normal (not zigzag) order.
  UBYTE2 data_values [JPEGSAMPLESIZE] ;
  // The scaled quantization values stored in normal (not zigzag) order.
  double float_scaling [JPEGSAMPLESIZE] ;

  bool is_used ; // Set to true if any component uses this table.

  friend class JpegEncoderDataUnit ;
} ;

inline bool JpegEncoderQuantizationTable::isUsed () const
{
  return is_used ;
}

inline void JpegEncoderQuantizationTable::isUsed (bool value)
{
  is_used = value ;
  return ;
}


inline UBYTE2 JpegEncoderQuantizationTable::operator[](unsigned int index) const
{
  return data_values [index] ;
}

inline UBYTE2 &JpegEncoderQuantizationTable::operator[](unsigned int index)
{
  return data_values [index] ;
}

} // End Namespace ColosseumPrivate

#endif
