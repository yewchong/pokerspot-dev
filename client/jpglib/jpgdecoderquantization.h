#ifndef CB__JPDEQUAN_H
#define CB__JPDEQUAN_H
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
//  Title:  JPEG Decoder Quantization Table Class Implementation
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class represents a quantization table used by the
//    JPEG decoder.
//

#include <iostream>
#include "jpgpvt.h"
#include "bitmapimage.h"

namespace Colosseum
{
class JpegInputStream ;
}

namespace ColosseumPrivate
{

class JpegDecoderQuantizationTable
{
public:
  JpegDecoderQuantizationTable() ;
  ~JpegDecoderQuantizationTable() {}

  // This function tells the caller if the quantization table has been
  // defined by the JPEG input stream.
  bool tableDefined () const ;

  // Function to read the quantization table from the input stream.
  void readTable (Colosseum::JpegInputStream &decoder, unsigned int precision) ;

  // This function builds the scaled quantization tables used in
  // fast IDCT implementations.
  void buildScaledTables () ;

  // This function prints the contents of the quantization table to
  // an output stream.
  void printOn (std::ostream &) const ;


  // Scaling factor used for the scaled integer values.
  enum { QuantizationIntegerScale = 12, } ;


private:
  // Dummy Declarations for Required Member Functions
  JpegDecoderQuantizationTable (const JpegDecoderQuantizationTable&) ;
  JpegDecoderQuantizationTable &operator=(const JpegDecoderQuantizationTable&) ;

  // Quantization Values in Zig-Zag Order.
  UBYTE2 data_values [JPEGSAMPLESIZE] ;


  // Scaled quantization values used for the fast IDCT implementations.
  double float_scaling [JPEGSAMPLESIZE] ;
  long integer_scaling [JPEGSAMPLESIZE] ;

  // This flag gets set to true when the quantization is defined in the
  // JPEG input stream. It is used to ensure that an compressed scan does
  // not attempt to use an undefined quantization table.
  bool table_defined ;

  friend class JpegDecoderDataUnit ;
} ;

inline bool JpegDecoderQuantizationTable::tableDefined () const
{
  return table_defined ;
}

} // End Namespace ColosseumPrivate

inline std::ostream &operator<<(std::ostream &strm,
                                const ColosseumPrivate::JpegDecoderQuantizationTable &du)
{
  du.printOn (strm) ;
  return strm ;
}

#endif
