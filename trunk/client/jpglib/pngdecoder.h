#ifndef CB__PNGDECODER_H
#define CB__PNGDECODER_H
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
//  Title:  PNG Decoder class definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class is a decoder for PNG images
//

#include "bitmapimage.h"
#include "pnginputstream.h"
#include "inflatedecoder.h"

namespace Colosseum
{
class PngInputStream ;

class PngDecoder : public BitmapImageDecoder
{
public:
  PngDecoder () ;
  virtual ~PngDecoder () ;

  virtual void readImage (PngInputStream &, BitmapImage &) ;
  virtual void readImageFile (const std::string &, BitmapImage &) ;

  void setVerbose (bool) ;
  bool getVerbose () const ;

  class PngError ;
  class BadPngStream ;
  
private:
  PngDecoder (const PngDecoder &) ;
  PngDecoder &operator=(PngDecoder &) ;

  struct InterlaceInfo
  {
    unsigned int row_interval ;
    unsigned int col_interval ;
    unsigned int start_row ;
    unsigned int start_col ;
  } ;


  // Function to read the PNG signature.
  void readSignature (PngInputStream &inputstream) ;

  // Functions for processing chunks
  void processChunk (PngInputStream &inputstream, BitmapImage &image) ;
  void processHeader (PngInputStream &inputstream, BitmapImage &image) ;
  void processPalette (PngInputStream &inputstream) ;
  void processBackground (PngInputStream &inputstream) ;
  void processGamma (PngInputStream &inputstream) ;
  void processData (PngInputStream &inputstream, BitmapImage &image) ;
  void processChromaticities (PngInputStream &inputstream) ;
  void processHistogram (PngInputStream &inputstream) ;
  void processPhysicalPixelDimensions (PngInputStream &inputstream) ;
  void processSignificantBits (PngInputStream &inputstream) ;
  void processTextualData (PngInputStream &inputstream) ;
  void processImageTime (PngInputStream &inputstream) ;
  void processTransparency (PngInputStream &inputstream) ;
  void processCompressedText (PngInputStream &inputstream) ;

  void callProgressFunction (unsigned int percent) ;

  void freeData () ; // Dynamic Memory Release

  // Function for reading pixel data for the image.
  void readPixelData (PngInputStream &inputstream, BitmapImage &image) ;
  void copyNoninterlacedRowToImage (unsigned int row, BitmapImage &image);
  void copyInterlacedRowToImage (unsigned int row, unsigned int width, BitmapImage &image) ;
  void readNoninterlaced (PngInputStream &inputstream, BitmapImage &image) ;
  void readInterlaced (PngInputStream &inputstream, BitmapImage &image) ;
  void filterRow (unsigned int filter) ;

  bool verbose_flag ;
  bool reading_pixel_data ;  // Set to true while reading IDAT blocks

  // IDAT read state
  unsigned int bit_offset ;    // Bit offset into current byte.
  unsigned int byte_offset ;   // Byte offset into current IDAT chunk
  UBYTE1 bit_buffer ;          // IDAT read state

  // Image information from the header
  UBYTE4 image_height ;
  UBYTE4 image_width ;
  UBYTE1 image_depth ;  // Bits per component vlaue
  unsigned int image_color_type ;
  UBYTE1 image_compression_method ;
  UBYTE1 image_filter_method ;
  UBYTE1 image_interlace_method ;

  // Row input buffers
  UBYTE1 *row_buffers [2] ;
  int current_row_buffer ;
  unsigned int row_buffer_width ;

  unsigned int interlace_pass ;   // Current interlace pass
  double file_gamma ;             // Value from a gAMAchunk

  // Values used to parse the chunk ordering.
  bool data_read ; // False until IDAT chunk read.
  bool palette_read ; // False until PLTE chunk read.
  bool header_read ; // False until IHDR chunk read.
  bool end_read ; // False until IEND chunk read.

  // Number of passes in an interlaced image.
  enum { InterlacePasses = 7 } ;

  static const InterlaceInfo interlace_values [] ;

  InflateDecoder inflate_decoder ;

  unsigned int background_gray ;
  unsigned int background_red ;
  unsigned int background_green ;
  unsigned int background_blue ;

  unsigned int palette_size ;
  struct
  {
    UBYTE1 red ;
    UBYTE1 green ;
    UBYTE1 blue ;
  } color_palette [256] ;
} ;

class PngDecoder::PngError : public GraphicsException
{
public:
  PngError (const std::string &msg) : GraphicsException (msg) {}
} ;


class PngDecoder::BadPngStream : public PngDecoder::PngError
{
public:
  BadPngStream (const std::string &msg) : PngDecoder::PngError (msg) {}  
} ;


} // End Namespace 

#endif
