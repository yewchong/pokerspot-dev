#ifndef CB__JPDECOMP_H
#define CB__JPDECOMP_H
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
//  Title:  JPEG Decoder Component Class Definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class represents a component within the JPEG decoder.
//


#include "jpgpvt.h"
#include "jfif.h"
#include "jpgdecoderdataunit.h"
#include "jpghuffmandecoder.h"
#include "jpgcoefficient.h"
#include "bitmapimage.h"

namespace Colosseum
{
class JpegInputStream ;
class JpegDecoder ;
}

namespace ColosseumPrivate
{
class JpegDecoderComponent
{
public:
  JpegDecoderComponent () ;
  ~JpegDecoderComponent () ;

  // We have made the color conversions static because RGB
  // conversion requires the data from three components.
  // Grayscale conversion is static strictly for consistency
  // with RGB.
  static void convertRgb (JpegDecoderComponent &c1,
                          JpegDecoderComponent &c2,
                          JpegDecoderComponent &c3,
                          Colosseum::BitmapImage &image) ;
  static void convertGrayscale (JpegDecoderComponent &cc, Colosseum::BitmapImage &image) ;

  unsigned int horizontalFrequency () const ;
  void horizontalFrequency (unsigned int) ;

  unsigned int verticalFrequency () const ;
  void verticalFrequency (unsigned int) ;

  void setQuantizationTable (JpegDecoderQuantizationTable &table) ;
  void allocateComponentBuffers (const Colosseum::JpegDecoder &decoder) ;
  void freeComponentBuffers () ;
  void setHuffmanTables (JpegHuffmanDecoder &dc, JpegHuffmanDecoder &ac) ;
  void upsampleImage (bool usefilter) ;

  void checkAcTable () ;
  void checkDcTable () ;
  void checkQuantizationTable () ;

  void decodeSequential (Colosseum::JpegInputStream &inputstream,
                         unsigned int mcurow,
                         unsigned int mcucol) ;

  unsigned long noninterleavedRows () const ;
  unsigned long noninterleavedCols () const ;
  void resetDcDifference () ;

  void decodeDcFirst (Colosseum::JpegInputStream &inputstream,
                                    unsigned int row,
                                    unsigned int col,
                                    unsigned int ssa) ;
  void decodeDcRefine (Colosseum::JpegInputStream &inputstream,
                                     unsigned int row,
                                     unsigned int col,
                                     unsigned int ssa) ;

  void decodeAcFirst (Colosseum::JpegInputStream &inputstream,
                                    unsigned int row,
                                    unsigned int col,
                                    unsigned int sss,
                                    unsigned int sse,
                                    unsigned int ssa) ;

  void decodeAcRefine (Colosseum::JpegInputStream &inputstream,
                                     unsigned int row,
                                     unsigned int col,
                                     unsigned int sss,
                                     unsigned int sse,
                                     unsigned int ssa) ;

  void progressiveInverseDct () ;

private:
  // Dummy Declarations for Required Member Functions
  JpegDecoderComponent (const JpegDecoderComponent &) ;
  JpegDecoderComponent operator=(const JpegDecoderComponent &) ;

  void upSample1To1 () ;
  void blockFilter () ;
  void triangleFilter () ;

  void extendRightColumn () ;
  void extendBottomRow () ;

  // Jfif/Frame component ID
  unsigned int component_id ;

  // Sampling Frequencies
  unsigned int horizontal_frequency ;
  unsigned int vertical_frequency ;

  // These values are the numnber of samples to take for each data
  // point. They come from the sampling frequencies and the maximum
  // sampling frequencies of all the components in the image.
  // sampling frequencies of all the components in the image.
  unsigned int v_sampling ;
  unsigned int h_sampling ;

  // Last encoded DC value.
  int last_dc_value ;

  // Entropy tables used by the component.
  JpegHuffmanDecoder *ac_table ;
  JpegHuffmanDecoder *dc_table ;

  // Quantization table used by the component
  JpegDecoderQuantizationTable *quantization_table ;

  // End of band Run - Progressive Specific
  unsigned int eob_run ;

  // Non-interleaved dimensions.
  unsigned int noninterleaved_rows ;
  unsigned int noninterleaved_cols ;

  unsigned int du_rows ;
  unsigned int du_cols ;

  JpegDecoderDataUnit *data_units ;
  JPEGSAMPLE *upsample_data ;
  JpegCoefficientBlock *coefficient_blocks ;

  unsigned int image_height ;
  unsigned int image_width ;

} ;

inline unsigned int JpegDecoderComponent::horizontalFrequency () const
{
  return horizontal_frequency ;
}

inline unsigned int JpegDecoderComponent::verticalFrequency () const
{
  return vertical_frequency ;
}

inline unsigned long JpegDecoderComponent::noninterleavedRows () const
{
  return noninterleaved_rows ;
}

inline unsigned long JpegDecoderComponent::noninterleavedCols () const
{
  return noninterleaved_cols ;
}

inline void JpegDecoderComponent::resetDcDifference ()
{
  last_dc_value = 0 ;
  return ;
}


}
#endif
