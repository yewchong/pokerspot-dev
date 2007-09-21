#ifndef CB__JPEGDECO_H
#define CB__JPEGDECO_H
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
//  Title:  JPEG Decoder Class Implementation
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//


#include <iostream>

#include "systemspecific.h"
#include "bitmapimage.h"
#include "jpginputstream.h"

namespace ColosseumPrivate
{
class JpegDecoderComponent ;
class JpegHuffmanDecoder ;
class JpegDecoderQuantizationTable ;
}

namespace Colosseum
{

class JpegDecoder : public BitmapImageDecoder
{
public:
  JpegDecoder () ;
  JpegDecoder (const JpegDecoder &) ;
  virtual ~JpegDecoder () ;
  JpegDecoder &operator=(const JpegDecoder &) ;

  virtual void readImage (JpegInputStream &stream, BitmapImage &image) ;
  bool getVerbose () const ;
  void setVerbose (bool) ;

  bool getUseFilters () const ;
  void setUseFilters (bool) ;

  virtual void updateImage () ;

  virtual void readImageFile (const std::string &, BitmapImage &image) ;

  class JpegError ;
  class JpegBadStream ;

protected:
  // While we don't expect classes to be derived from this one,
  // we have placed the functions we expect friend classes to
  // call here.

  bool isProgressive () const ;
  unsigned int mcuRows () const ;
  unsigned int mcuCols () const ;
  unsigned int frameHeight () const ;
  unsigned int frameWidth () const ;
  unsigned int maxVFrequency () const ;
  unsigned int maxHFrequency () const ;

  unsigned int restartInterval () const ;

private:
  void initialize () ;
  void doCopy (const JpegDecoder &) ;
  void callProgressFunction (unsigned int progress) ;
  void getScanCount (JpegInputStream &inputstream) ;

  void readStreamHeader (JpegInputStream &inputstream) ;
  void readMarker (JpegInputStream &inputstream) ;
  void readApplication (UBYTE1 type, JpegInputStream &inputstream) ;
  void readHuffmanTable (JpegInputStream &inputstream) ;
  void readQuantization (JpegInputStream &inputstream) ;
  void readRestartInterval (JpegInputStream &inputstream) ;
  void readStartOfFrame (JpegInputStream &inputstream, UBYTE1 type) ;
  void readStartOfScan (JpegInputStream &inputstream) ;
  void calculateMcuDimensions ();
  bool scanIsInterleaved () const ;

  void readProgressiveScanData (JpegInputStream &inputstream,
                                unsigned int sss, unsigned int sse,
                                unsigned int sah, unsigned int sal) ;
  void readDcFirst (JpegInputStream &inputstream, unsigned int ssa) ;
  void readDcRefine (JpegInputStream &inputstream, unsigned int ssa) ;
  void readAcFirst (JpegInputStream &inputstream, unsigned int sss, unsigned int sse, unsigned int ssa) ;
  void readAcRefine (JpegInputStream &inputstream,
                     unsigned int sss,
                     unsigned int sse,
                     unsigned int ssa) ;
  void freeAllocatedResources () ;

  void readSequentialScanData (JpegInputStream &inputstream) ;

  void readSequentialInterleavedScan (JpegInputStream &inputstream) ;
  void readSequentialNonInterleavedScan (JpegInputStream &inputstream) ;
  void resetDcDifferences () ;
  void processRestartMarker (JpegInputStream &inputstream) ;

  void refineAcCoefficient (JpegInputStream &inputstream, BYTE2 &value, unsigned int ssa) ;

  // Huffman tables
  ColosseumPrivate::JpegHuffmanDecoder *ac_tables ;
  ColosseumPrivate::JpegHuffmanDecoder *dc_tables ;

  // Quantization tables
  ColosseumPrivate::JpegDecoderQuantizationTable *quantization_tables ;

  // Bit I/O state
  int bit_position ;        // Called CNT in Section F.2.2.5
  unsigned char bit_data ;  // Called B in Section F.2.2.5

  bool eoi_found ;
  bool sof_found ;

  bool verbose_flag ;

  unsigned int restart_interval ;

  unsigned int frame_width ;
  unsigned int frame_height ;
  unsigned int frame_type ;

  unsigned max_horizontal_frequency ;
  unsigned max_vertical_frequency ;

  bool progressive_frame ;

  bool use_filters ;

  unsigned int component_count ;
  ColosseumPrivate::JpegDecoderComponent *components ;
  unsigned int *component_indices ;

  // Address of the image that is currently being processed.
  BitmapImage *current_image ;

  // Progress Counters
  unsigned int current_scan ;
  unsigned int scan_count ;

  unsigned int mcu_rows ;
  unsigned int mcu_cols ;

  unsigned int mcu_height ;
  unsigned int mcu_width ;

  unsigned int scan_component_count ;
  ColosseumPrivate::JpegDecoderComponent **scan_components ;

  unsigned int expected_restart ;

  bool strict_jfif ;

  // This flag is available to setXYZ functions to prevent the changing
  // of parameters during decompression that could screw things up.
  bool processing_image ;

  friend class ColosseumPrivate::JpegDecoderQuantizationTable ;
  friend class ColosseumPrivate::JpegHuffmanDecoder ;
  friend class ColosseumPrivate::JpegDecoderComponent ;
} ;


class JpegDecoder::JpegError : public GraphicsException
{
public:
  JpegError (const std::string &msg) : GraphicsException (msg) {}
} ;

class JpegDecoder::JpegBadStream : public JpegDecoder::JpegError
{
public:
  JpegBadStream (const std::string &msg) : JpegDecoder::JpegError (msg) {}
} ;


} // End Namespace

#endif
