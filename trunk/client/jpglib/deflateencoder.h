#ifndef CB__DEFLATEENCODER_H
#define CB__DEFLATEENCODER_H

//
//  Title:  Deflate Encoder Class
//
//  Copyright 1999, Colosseum Builders, Inc.
//  All rights reserved.
//
//  Colosseum Builders, Inc. makes no warranty, expressed or implied
//  with regards to this software. It is provided as is.
//
//  Author:  John M. Miano (miano@colosseumbuilders.com)
//
//  Date:    Ma7 10, 1999
//
//  Version: 1
//
//  Description:
//
//    A class for implementing Deflate compression.
//

#include <iostream>
#include "systemspecific.h"
#include "deflatehuffmanencoder.h"

namespace Colosseum
{

class DeflateOutputStream ;

class DeflateEncoder 
{
public:
  DeflateEncoder () ;
  virtual ~DeflateEncoder () ;

  void startCompressedStream (DeflateOutputStream &) ;
  void compressData (DeflateOutputStream & outputstream, 
                     const char data[], 
                     unsigned int size) ;
  void endCompressedStream (DeflateOutputStream &) ;

  enum CompressionLevel
  {
    FASTESTCOMPRESSION = 0,
    FASTCOMPRESSION = 1,
    DEFAULTCOMPRESSION = 2,
    MAXIMUMCOMPRESSION = 3,
  } ;

  void setCompressionLevel (CompressionLevel) ;
  CompressionLevel getCompressionLevel () const ;
    
  unsigned long getBlockSize () const ;
  void setBlockSize (unsigned long) ;

  class DeflateError ;

private:
  DeflateEncoder (const DeflateEncoder &) ;
  DeflateEncoder &operator=(const DeflateEncoder &) ;

  struct HashEntry
  {
    UBYTE2 index ;      // Index into the LZ Window
    HashEntry *next ;   // Next collision entry
    HashEntry *previous ;
  } ;

  void prepareNewBlock () ;
  void longestMatch (unsigned int count,
                     unsigned int &length,
                     unsigned int &offset) ;
  unsigned int hashValue (unsigned int) ;
  void moveHashEntry (unsigned int entry, unsigned int hashvalue) ;
  typedef void (DeflateEncoder::*LENGTHFUNCTION) (DeflateOutputStream &outputstream,
                                              unsigned int index,
                                              unsigned int code,
                                              unsigned int length,
                                              unsigned int extra) ;
  void gatherLengthCounts (DeflateOutputStream &outputstream,
                           unsigned int,
                           unsigned int code,
                           unsigned int,
                           unsigned int) ;
  void outputLengthCounts (DeflateOutputStream &outputstream,
                           unsigned int index,
                           unsigned int code,
                           unsigned int extra,
                           unsigned int value) ;
  void findLengthCodes (DeflateOutputStream &outputstream,
                        LENGTHFUNCTION, 
                        ColosseumPrivate::DeflateHuffmanEncoder &encoder, 
                        unsigned int count) ;
  bool compressDataBlock () ;
  void compressLookaheadData (DeflateOutputStream &outputstream, 
                              unsigned int count, 
                              bool endflag) ;

  void outputBlock (const void *data, unsigned int length) ;
  void freeBuffers () ;
  void fillBuffer (unsigned int count) ;
  void initializeHashTable () ;
  void outputDeflateHeader (DeflateOutputStream &outputstream, bool lastblock) ;
  void outputZLibHeader (DeflateOutputStream &outputstream) ;
  void outputBlockData (DeflateOutputStream &outputstream) ;

  // Array of have table entries, one entry for each position in the LZ Window.
  HashEntry *hash_values ;
  // Hash Table
  HashEntry *hash_table ;

  ColosseumPrivate::DeflateHuffmanEncoder distance_table ;
  ColosseumPrivate::DeflateHuffmanEncoder length_table ;
  ColosseumPrivate::DeflateHuffmanEncoder length_length_table ;

  // Compression vs. Time selected by the user.
  CompressionLevel compression_level ;
  // Number of hash entries to search for a match. 
  // This is the internal translation of compression_level.
  unsigned int search_limit ; 

  UBYTE4 adler_value ;
  UBYTE1 *lz_window ;
  UBYTE1 *lookahead_buffer ;
  // Index of next position to add to the lookahead_buffer.
  unsigned int lookahead_input_position ;
  // Index of next position to remove from the lookahead_buffer.
  unsigned int lookahead_output_position ;
  // Current position in the lz_window buffer.
  unsigned int lz_position ;

  // LZ Compressed Block
  UBYTE2 *block_buffer ;
  // Size of Block Buffer
  unsigned int block_buffer_size ;
  // Number of entries added to the block buffer so far.
  unsigned int block_buffer_count ;

  // false => Compression not underway
  // true => Compression underway
  bool compression_in_progress ;
} ;

class DeflateEncoder::DeflateError : public std::exception
{
private:
  std::string message ;
public:
  DeflateError (const std::string &msg) : message (msg) {} ;
  DeflateError (const DeflateError &source) : message (source.message) {}
  DeflateError &operator=(const DeflateError &source) 
    {
      if (&source != this)
      {
        message = source.message ;
      }
      return *this ;
    }
  virtual const char *what () const throw () { return message.c_str () ; } 
} ;


} // End Namespace Colosseum


#endif
