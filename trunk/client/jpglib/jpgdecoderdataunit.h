#ifndef CB__JPDEDU_H
#define CB__JPDEDU_H
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

//
//  Title:  JPEG Definitions and Utility Functions
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//


//
//  Decoder Data Unit Class Definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Descrition:
//
//    The DataUnit class represents an 8x8 sample block for one
//    component of the JPEG image.
//
//

#include <iostream>

#include "jpgpvt.h"
#include "jpgdecoderquantization.h"
#include "jpgcoefficient.h"

namespace ColosseumPrivate
{


class JpegDecoderDataUnit
{
public:
  // Declaration of a type for pointers to member functions
  // for implementing the IDCT. The input parameters are
  // The IDCT coefficients and the [de]quantization table.
  typedef JpegDecoderDataUnit &(JpegDecoderDataUnit::*IDctFunction) (
                                const JpegCoefficientBlock,
                                const JpegDecoderQuantizationTable  &) ;

  JpegDecoderDataUnit() {}
  virtual ~JpegDecoderDataUnit () {}

  // Utility function to write the IDCT values to an output stream.
  void printOn (std::ostream &) const ;

  // General IDCT Function
  JpegDecoderDataUnit &JpegDecoderDataUnit::inverseDCT (
                        const JpegCoefficientBlock cb,
                        const JpegDecoderQuantizationTable  &qt) ;

  // These are the IDCT implementations.
  JpegDecoderDataUnit &floatInverseDCT (const JpegCoefficientBlock,
                             const JpegDecoderQuantizationTable  &) ;
  JpegDecoderDataUnit &integerInverseDCT (const JpegCoefficientBlock,
                               const JpegDecoderQuantizationTable  &) ;

  // Operators to retrieve the individual IDCT values.

  JPEGSAMPLE *operator [] (unsigned int ii) ;
  JPEGSAMPLE const* operator [] (unsigned int ii) const ;

private:
  // Dummy Declarations For Required Members Functions 
  JpegDecoderDataUnit (const JpegDecoderDataUnit &) ;
  JpegDecoderDataUnit &operator=(const JpegDecoderDataUnit &) ;

  // The IDCT values.
  UBYTE1 values [JPEGSAMPLEWIDTH][JPEGSAMPLEWIDTH] ;

  // This is a pointer to the member function that implements
  // the desired IDCT function.
  static IDctFunction idct_function ;
} ;

inline JPEGSAMPLE *JpegDecoderDataUnit::operator [] (unsigned int ii)
{
 return values [ii] ;
}

inline JPEGSAMPLE const* JpegDecoderDataUnit::operator [] (unsigned int ii) const
{
  return values [ii] ;
}

inline JpegDecoderDataUnit &JpegDecoderDataUnit::inverseDCT (
                        const JpegCoefficientBlock cb,
                        const JpegDecoderQuantizationTable  &qt)
{
  return (this->*idct_function) (cb, qt) ;
}


} // End Namespace ColosseumPrivate

inline std::ostream &operator<<(std::ostream &strm, ColosseumPrivate::JpegDecoderDataUnit &du) 
{
  du.printOn (strm) ;
  return strm ;
}



#endif
