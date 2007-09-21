#ifndef CB_ENCODERDATAUNIT_H
#define CB_ENCODERDATAUNIT_H
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
//  Title:  JpegEncoderDataUnit class definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class represents a data unit in the JPEG encoder.
//

#include "jpgpvt.h"
#include "jpgcoefficient.h"

#include "jpgencoderquantization.h"

namespace ColosseumPrivate
{

class JpegEncoderDataUnit
{
public:
  typedef JPEGSAMPLE DATAUNITVALUE ;

  JpegEncoderDataUnit () {} ;
  ~JpegEncoderDataUnit () {} ; //Must not be virtual.

  DATAUNITVALUE *operator[] (unsigned int) ;
  DATAUNITVALUE &ZigZagInput (unsigned int) ;

  void ForwardDct (JpegEncoderQuantizationTable &,
                   JpegCoefficientBlock &) ;

private:
  JpegEncoderDataUnit (const JpegEncoderDataUnit &) ;
  JpegEncoderDataUnit &operator=(const JpegEncoderDataUnit &) ;

  DATAUNITVALUE data [JPEGSAMPLEWIDTH][JPEGSAMPLEWIDTH] ;
} ;

inline JpegEncoderDataUnit::DATAUNITVALUE *JpegEncoderDataUnit::operator [](unsigned int ii)
{
  return data [ii] ;
}

inline JpegEncoderDataUnit::DATAUNITVALUE &JpegEncoderDataUnit::ZigZagInput (unsigned int ii)
{
  unsigned int order = JpegZigZagInputOrder (ii) ;
  return ((DATAUNITVALUE *) data) [order] ; 
}

} // End Namespace ColosseumPrivate

#endif
