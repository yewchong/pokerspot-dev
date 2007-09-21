#ifndef CB__JPEGENCODER_H
#define CB__JPEGENCODER_H
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
//  JPEG Encoder Library.
//
//  Title: JpegEncoder class definition.
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//
//  Description:
//
//    This class is an encoder for JPEG image. The process for using this
//    class is to call the property functions to set the attributes for the
//    stream then call the writeImage function to create a JPEG stream with
//    those attributes.
//

#include "bitmapimage.h"
#include "jpgencodercomponent.h"

namespace ColosseumPrivate
{
class JpegHuffmanEncoder ;
}

namespace Colosseum
{
class JpegOutputStream ;

class JpegEncoder : public BitmapImageEncoder
{
public:
  JpegEncoder () ;
  virtual ~JpegEncoder () ;
  // Required Member Functions
  JpegEncoder (const JpegEncoder &) ;
  JpegEncoder &operator=(const JpegEncoder &) ;

  virtual void writeImage (JpegOutputStream &outputstream,
                           const BitmapImage &image) ;
  virtual void writeImageFile (const std::string &, const BitmapImage &) ;

  //************************
  //** Property Functions **
  //************************

  // Image Quality (1-100)
  unsigned int getQuality () const ;
  void setQuality (unsigned int) ;

  // Grayscale Mode (True=Gray Scale, False=Color)
  bool getGrayscale () const ;
  void setGrayscale (bool) ;

  // Progressive Mode (True=Progressive, False=Sequential)
  bool getProgressive () const ;
  void setProgressive (bool) ;

  // Number of rows between restart markers (0=> No restart markers)
  unsigned int getRowsPerRestart () const ;
  void setRowsPerRestart (unsigned int) ;

  // Comment String
  std::string getComment () const ;
  void setComment (const std::string &) ;

  // Component Sampling Frequencies (1-4)
  void setSamplingFrequency (unsigned int component, unsigned int hf, unsigned int vf) ;
  void getSamplingFrequency (unsigned int component, unsigned int &hf, unsigned int &vf) ;

  // Scan Attributes {scan number, component bit mask,
  // spectral selection end (0-63), successive approximation (0-13) }
  void setScanAttributes (unsigned int scan, 
                          unsigned long components, 
                          unsigned int sse, unsigned int ssa) ;
  void getScanAttributes (unsigned int scan, 
                          unsigned long &components, 
                          unsigned int &sse, unsigned int &ssa) ;

  // Maximum number of scans
  enum { MAXSCANS = 256, } ;

  // Component Identifiers
  enum { YCOMPONENT = 1, CBCOMPONENT = 2, CRCOMPONENT = 3, } ;

  class JpegError ;

protected:
  void callProgressFunction (unsigned int) ;

private:
  void doCopy (const JpegEncoder &) ;
  void initialize () ;

  // This structure is used to represent a scan.
  struct Scan
  {
    // Bitmap of components in the scan.
    unsigned long component_mask ;
    // Spectral Selection for the scan.
    unsigned int spectral_selection_start ;
    unsigned int spectral_selection_end ;
    // Successive Approximation for the first iteration of the scan
    unsigned int successive_approximation ;
    // Successive Approximation Progress. Updated as scans are output.
    int successive_approximation_high ;
    int successive_approximation_low ;
  } ;

  // This function determines if the output scan parameters are valid. It
  // throwse the EJpegError exception an inconsistency is found.
  void validateParameters () ;

  void outputMarker (JpegOutputStream &outputstream, ColosseumPrivate::JpegMarker) ;

  // Block Output Methods
  void printQuantizationTables (JpegOutputStream &outputstream) ;
  void printSequentialFrame (JpegOutputStream &outputstream, const BitmapImage &image) ;
  void printProgressiveFrame (JpegOutputStream &outputstream, const BitmapImage &image) ;
  void printComment (JpegOutputStream &outputstream, const std::string &) ;
  void outputJfifHeader (JpegOutputStream &outputstream) ;

  void outputRestartInterval (JpegOutputStream &outputstream,
                              unsigned int restartinterval) ;

  void printHuffmanTables (JpegOutputStream &outputstream, 
                           const Scan &scan, 
                           bool usedc, 
                           bool useac) ;

  // Sequential Scan Output
  void printSequentialScan (JpegOutputStream &outputstream, const Scan &scan) ;

  // Progressive Output Functions
  void printProgressiveScan (JpegOutputStream &outputstream, const Scan &scan) ;
  void printDcFirst (JpegOutputStream &outputstream, const Scan &scan) ;
  void printDcRefine (JpegOutputStream &outputstream, const Scan &scan) ;
  void printAcFirst (JpegOutputStream &outputstream, const Scan &scan) ;
  void printAcRefine (JpegOutputStream &outputstream, const Scan &scan) ;
  void firstAcData (JpegOutputStream &outputstream, const Scan &scan, bool outputrestarts, ColosseumPrivate::JpegEncoderComponent::ACOUTPUTFUNCTION acfunction) ;
  void refineAcData (JpegOutputStream &outputstream, const Scan &scan, bool outputrestarts, ColosseumPrivate::JpegEncoderComponent::ACOUTPUTFUNCTION acfunction) ;


  void interleavedPass (
               JpegOutputStream &outputstream,
               bool writedata,
               ColosseumPrivate::JpegEncoderComponent::COMPONENTPASSFUNCTION passfunction,
               ColosseumPrivate::JpegEncoderComponent::DCOUTPUTFUNCTION dcfunction,
               ColosseumPrivate::JpegEncoderComponent::ACOUTPUTFUNCTION acfunction,
               unsigned int sss,
               unsigned int sse,
               unsigned int ssa) ;
  void noninterleavedPass (
               JpegOutputStream &outputstream,
               bool writedata,
               ColosseumPrivate::JpegEncoderComponent::COMPONENTPASSFUNCTION passfunction,
               ColosseumPrivate::JpegEncoderComponent::DCOUTPUTFUNCTION dcfunction,
               ColosseumPrivate::JpegEncoderComponent::ACOUTPUTFUNCTION acfunction,
               unsigned int sss,
               unsigned int sse,
               unsigned int ssa) ;

  void resetDcValues () ;
  void calculateMcuDimensions () ;
  void countPassesForProgressReporting () ;

  void findComponentsInScan (Scan &scan) ;

  void outputRestartMarker (JpegOutputStream &outputstream, int) ;

  // Quantization Tables
  ColosseumPrivate::JpegEncoderQuantizationTable *quantization_tables ;

  // Huffman Tables
  ColosseumPrivate::JpegHuffmanEncoder *ac_tables ;
  ColosseumPrivate::JpegHuffmanEncoder *dc_tables ;

  // Properties
  bool gray_scale ;
  unsigned int rows_per_restart ;
  unsigned int restart_interval ;
  unsigned int image_quality ;
  std::string comment_string ;
  bool progressive_mode ;

  unsigned int total_passes ;
  unsigned int current_pass ;

  // Image Size
  unsigned int frame_width ;
  unsigned int frame_height ;
  // Maximum Frequencies in all components
  unsigned int max_horizontal_frequency ;
  unsigned int max_vertical_frequency ;
  // MCU Dimensions
  unsigned int  mcu_rows ;
  unsigned int  mcu_cols ;

  unsigned int scan_count ;

  // Scan Descriptors
  Scan image_scans [MAXSCANS] ;
  // Components
  enum { MAXCOMPONENTS = 4, } ;
  ColosseumPrivate::JpegEncoderComponent image_components [MAXCOMPONENTS] ;
  // Index of the quantization table assigned to each component.
  unsigned int quantization_table_assignments [MAXCOMPONENTS] ;

  // Components used in the scan being processed.
  unsigned int scan_component_count ;
  ColosseumPrivate::JpegEncoderComponent *scan_components [ColosseumPrivate::JPEGMAXCOMPONENTSPERSCAN] ;

  friend class ColosseumPrivate::JpegEncoderComponent ;
} ;

//
//  This function calls the progress function if one has been specified.
//
//  Parameters:
//      progress: Percent completed (0-100)
//
inline void JpegEncoder::callProgressFunction (unsigned int progress) 
{
  if (progress_function == 0)
    return ;

  bool abort = false ;
  bool update = false;
  unsigned int percent = progress ;
  if (percent > 100)
    percent = 100 ;
  progress_function (*this,
                     progress_data,
                     current_pass,
                     total_passes,
                     percent,
                     abort) ;

  // See if the user wants to stop.
  if (abort)
    throw GraphicsAbort () ;
  return ;
}

class JpegEncoder::JpegError : public GraphicsException
{
public:
  JpegError (const std::string &msg) : GraphicsException (msg) {}
  JpegError (const JpegError &source) : GraphicsException (source) {} 
  virtual ~JpegError () throw () {}
} ;

} // End Namespace Colosseum

#endif

