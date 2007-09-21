#ifndef CB__JPENCOMP_H
#define CB__JPENCOMP_H
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
//  Title:  JpegEncoderComponent class definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class represents a single component while writing a JPEG image.
//

#include "bitmapimage.h"
#include "jpgcoefficient.h"
#include "jpgpvt.h"

namespace Colosseum
{
class JpegOutputStream ;
class JpegEncoder ;
}

namespace ColosseumPrivate
{

class JpegHuffmanEncoder ;
class JpegEncoderQuantizationTable ;

class JpegEncoderComponent
{
public:
  JpegEncoderComponent () ;
  virtual ~JpegEncoderComponent () ;

  void printAcData (int, int, int) ;
  void printDcData (int, int) ;
  void gatherAcData (int, int, int) ;
  void gatherDcData (int, int) ;

  typedef void (JpegEncoderComponent::*DCOUTPUTFUNCTION) (int, int) ;
  typedef void (JpegEncoderComponent::*ACOUTPUTFUNCTION) (int, int, int) ;
  typedef void (JpegEncoderComponent::*COMPONENTPASSFUNCTION) (
                            unsigned int row, unsigned int col,
                            DCOUTPUTFUNCTION, ACOUTPUTFUNCTION,
                            unsigned int sss, unsigned int sse,
                            unsigned int ssa) ;
  

  void encodeSequential (unsigned int row, unsigned int col,
                         DCOUTPUTFUNCTION dcfunction,
                         ACOUTPUTFUNCTION acfunction,
                         unsigned int, unsigned int, unsigned int) ;
  void progressiveDcFirst (unsigned int row, unsigned int col,
                           DCOUTPUTFUNCTION dcfunction, ACOUTPUTFUNCTION,
                           unsigned int, unsigned int,
                           unsigned int ssa) ;
  void progressiveDcRefine (unsigned int row, unsigned int col,
                            DCOUTPUTFUNCTION dcfunction, ACOUTPUTFUNCTION,
                            unsigned int, unsigned int,
                            unsigned int) ;
  void progressiveAcFirst (unsigned int row, unsigned int col,
                           ACOUTPUTFUNCTION acfunction,
                           unsigned int sss, unsigned int sse,
                           unsigned int ssa) ;
  void progressiveAcRefine (unsigned int row, unsigned int col,
                            ACOUTPUTFUNCTION acfunction,
                            unsigned int sss, unsigned int sse,
                            unsigned int ssa) ;

  void resetEobRun () ;
  void printEobRun (ACOUTPUTFUNCTION acfunction) ;
  void printRefineEobRun (ACOUTPUTFUNCTION acfunction,
                          unsigned int sss, unsigned int sse,
                          unsigned int ssa) ;
  unsigned int getHorizontalFrequency () const ;
  void setHorizontalFrequency (unsigned int) ;
  unsigned int getVerticalFrequency () const ;
  void setVerticalFrequency (unsigned int) ;

  unsigned int dataUnitRows () const ;
  unsigned int dataUnitCols () const ;
  void resetDcDifference () ;

  void setHuffmanTables (JpegHuffmanEncoder &dc,
                         JpegHuffmanEncoder &ac) ;
  void setQuantizationTable (JpegEncoderQuantizationTable &table) ;

  void freeDynamicStorage () ;

  void setOutputStream (Colosseum::JpegOutputStream &) ;

  static void rgbConvert (Colosseum::JpegEncoder &encoder, 
                          const Colosseum::BitmapImage &image, 
                          unsigned int maxhf, unsigned int maxvf,
                          JpegEncoderComponent &ycomponent,
                          JpegEncoderComponent &cbcomponent,
                          JpegEncoderComponent &crcomponent) ;
  static void grayScaleConvert (Colosseum::JpegEncoder &encoder,
                          const Colosseum::BitmapImage &image, 
                          JpegEncoderComponent &ycomponent) ;
  void sampleComponent (Colosseum::JpegEncoder &encoder, unsigned int maxhf,
                        unsigned int maxvf) ;

  void allocateComponentBuffer (unsigned int imagewidth,
                                unsigned int imageheight,
                                unsigned int maxhf,
                                unsigned int maxvf) ;

  unsigned int noninterleavedDuColumns () const ;
  unsigned int noninterleavedDuRows () const ;

private:
  JpegEncoderComponent (const JpegEncoderComponent &) ;
  JpegEncoderComponent &operator=(const JpegEncoderComponent &) ;

  void calculateDuDimensions (const Colosseum::BitmapImage &image, unsigned int maxhf, unsigned int maxvf) ;

  typedef JPEGSAMPLE (*RGBTOYCBCRFUNCTION)(
                         JPEGSAMPLE red,
                         JPEGSAMPLE green,
                         JPEGSAMPLE blue) ;
  void sample1to1Component (Colosseum::JpegEncoder &encoder) ;
  void sampleNtoNComponent (Colosseum::JpegEncoder &encoder,
                            unsigned int maxhf,
                            unsigned int maxvf) ;


  // DCT Coefficients and dimensions
  unsigned int du_rows ;
  unsigned int du_cols ;
  JpegCoefficientBlock *dct_coefficients ;

  // EOB-run in context
  unsigned int eob_run ;
  unsigned int eob_start_du_row ;
  unsigned int eob_start_du_col ;
  unsigned int eob_start_position ;

  // Sampling Frequencies and p
  unsigned int v_frequency ;
  unsigned int h_frequency ;

  // Huffman and Quantization tables
  JpegHuffmanEncoder *ac_table ;
  JpegHuffmanEncoder *dc_table ;
  JpegEncoderQuantizationTable *quantization_table ;
  Colosseum::JpegOutputStream *output_stream ;

  // Last DC value used to calculate DC differences
  int last_dc_value ;

  JPEGSAMPLE *component_buffer ;
  unsigned int component_buffer_rows ;
  unsigned int component_buffer_columns ;

  unsigned int noninterleaved_du_rows ;
  unsigned int noninterleaved_du_colummns ;

} ;

inline unsigned int JpegEncoderComponent::getHorizontalFrequency () const
{
  return h_frequency ;
}

inline unsigned int JpegEncoderComponent::getVerticalFrequency () const
{
  return v_frequency ;
}

inline unsigned int JpegEncoderComponent::dataUnitRows () const
{
  return du_rows ;
}

inline unsigned int JpegEncoderComponent::dataUnitCols () const
{
  return du_cols ;
}

inline void JpegEncoderComponent::resetDcDifference ()
{
  last_dc_value = 0 ;
  return ;
}

inline void JpegEncoderComponent::setHuffmanTables (JpegHuffmanEncoder &dc,
                                                    JpegHuffmanEncoder &ac)
{
  dc_table = &dc ;
  ac_table = &ac ;
  return ;
}


inline void JpegEncoderComponent::setQuantizationTable (JpegEncoderQuantizationTable &table)
{
  quantization_table = &table ;
  return ;
}


inline void JpegEncoderComponent::setOutputStream (Colosseum::JpegOutputStream &outputstream)
{
  output_stream = &outputstream ;
  return ;
}

inline unsigned int JpegEncoderComponent::noninterleavedDuColumns () const
{
  return noninterleaved_du_colummns ;
}

inline unsigned int JpegEncoderComponent::noninterleavedDuRows () const
{
  return noninterleaved_du_rows ;
}


}

#endif

