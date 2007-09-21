#ifndef CB__PNGENCODER_H
#define CB__PNGENCODER_H
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
//  Title: PNG Encoder Class Definition
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class implements PNG encoding.
//

#include "bitmapimage.h"
#include "deflateencoder.h"

namespace Colosseum
{
class PngOutputStream ;

class PngEncoder : public BitmapImageEncoder
{
public:
  PngEncoder () ;
  PngEncoder (const PngEncoder &) ;
  virtual ~PngEncoder () ;
  PngEncoder &operator=(const PngEncoder &) ;

  virtual void writeImageFile (const std::string &, const BitmapImage &) ;

  void writeImage (PngOutputStream &, const BitmapImage &) ;

  // Property functions for predefined tEXt strings.
  void setTitle (const std::string &) ;
  std::string getTitle () const ;

  void setAuthor (const std::string &) ;
  std::string getAuthor () const ;

  void setDescription (const std::string &) ;
  std::string getDescription () const ;

  void setCopyright (const std::string &) ;
  std::string getCopyright () const ;

  void setSoftware (const std::string &) ;
  std::string getSoftware () const ;

  void setDisclaimer (const std::string &) ;
  std::string getDisclaimer () const ;

  void setWarning (const std::string &) ;
  std::string getWarning () const ;

  void setSource (const std::string &) ;
  std::string getSource () const ;

  void setComment (const std::string &) ;
  std::string getComment () const ;

  void setCompressionLevel (DeflateEncoder::CompressionLevel) ;
  DeflateEncoder::CompressionLevel getCompressionLevel () const ;
    
  void setUseFilters (bool) ;
  bool getUseFilters () const ;

  unsigned long getBlockSize () const ;
  void setBlockSize (unsigned long) ;

  bool getUseAlphaChannel () const ;
  void setUseAlphaChannel (bool) ;

  class PngError ;

protected:
  void writeImageData (PngOutputStream &, const BitmapImage &) ;
  void freeBuffers () ;

  void writeText (PngOutputStream &outputstream,
                  const std::string &keyword, 
                  const std::string &value) ;
  void writeTextBlocks (PngOutputStream &outputstream) ;

  void callProgressFunction (unsigned int) ;

  unsigned int row_width ;
  std::string title_string ;
  std::string author_string ;
  std::string description_string ;
  std::string copyright_string ;
  std::string software_string ;
  std::string disclaimer_string ;
  std::string warning_string ;
  std::string source_string ;
  std::string comment_string ;

  enum { FILTERBUFFERCOUNT = 5, } ;
  UBYTE1 *filter_buffers [FILTERBUFFERCOUNT] ;
  unsigned int current_filter ;
  unsigned int filter_mask ;
  unsigned int filter_width ;
  void filterRow () ;

  void doWrite (PngOutputStream &outputstream, const BitmapImage &image) ;

  UBYTE1 *row_buffer ;
  DeflateEncoder deflate_encoder ;

  bool use_alpha_channel ;

  bool processing_image ;
} ;

class PngEncoder::PngError : public GraphicsException
{
public:
  PngError (const std::string &msg) : GraphicsException (msg) {}
  PngError (const PngError &source) : GraphicsException (source) {} 
  virtual ~PngError () throw () {}
} ;


}


#endif


