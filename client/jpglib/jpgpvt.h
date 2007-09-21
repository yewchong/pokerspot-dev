#ifndef CB__JPGPVT_H
#define CB__JPGPVT_H
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
//  Title:  JPEG Definitions and Utility Functions
//
//  Author:  John M. Miano  miano@colosseumbuilders.com
//

#include "systemspecific.h"
#include "checks.h"

namespace ColosseumPrivate
{
#define JPEGMARKER(code,value,text) code = value
enum JpegMarker
{
#include "jpgmkdef.h"
} ;

// Table B.5
const int JPEGMAXHUFFMANTABLES = 4 ;
// Table B.4
const int JPEGMAXQUANTIZATIONTABLES = 4 ;

// Table B.2
const int JPEGMAXCOMPONENTSPERFRAME = 255 ;
// Table B.3
const int JPEGMAXCOMPONENTSPERSCAN = 4 ;

// Table B.2
const int JPEGMINSAMPLINGFREQUENCY  = 1 ;
const int JPEGMAXSAMPLINGFREQUENCY = 4 ;


// A.1.3
const int JPEGSAMPLEWIDTH = 8 ;
const int JPEGSAMPLESIZE = JPEGSAMPLEWIDTH * JPEGSAMPLEWIDTH ;

// Functions to convert to Zig-Zag order
unsigned int JpegZigZagInputOrder (unsigned int item) ;
unsigned int JpegZigZagOutputOrder (unsigned int item) ;

// Datatype used to represent a sample value.
typedef UBYTE2 JPEG12BITSAMPLE ;
typedef UBYTE1 JPEG8BITSAMPLE ;
typedef JPEG8BITSAMPLE JPEGSAMPLE ; 

typedef BYTE2 JPEG12BITCOEFFICIENT ;
typedef BYTE1 JPEG8BITCOEFFICIENT ;
typedef JPEG8BITCOEFFICIENT JPEGCOEFFICIENT ;

const int JPEGMINSAMPLEVALUE  = 0 ;
const int JPEGMAX8BITSAMPLEVALUE = 255 ; 
const int JPEGMAX12BITSAMPLEVALUE = 4095 ;
const int JPEGMAXSAMPLEVALUE = JPEGMAX8BITSAMPLEVALUE ;
const int JPEG8BITMIDPOINTSAMPLEVALUE = 128 ;
const int JPEG12BITMIDPOINTSAMPLEVALUE = 2048 ;
const int JPEGMIDPOINTSAMPLEVALUE = JPEG8BITMIDPOINTSAMPLEVALUE ;

// Table B.5
const unsigned int JPEGMAXHUFFMANCODELENGTH = 16 ;
const unsigned int JPEGMAXNUMBEROF8BITHUFFMANCODES = 256 ;
const unsigned int JPEGMAXNUMBEROFHUFFMANCODES = JPEGMAXNUMBEROF8BITHUFFMANCODES ;
// B.2.3
const int JPEGMAXDATAUNITSPERMCU = 10 ;

// Section B.2.3 Table B.3
const int JPEGMAXSUCCESSIVAPPROXIMATION = 13 ;

// Section B.2.4.1 Table B.4

const int JPEGMAX8BITQUANTIZATIONVALUE = 255 ;
const int JPEGMINQUANTIZATIONVALUE = 1 ;

extern const unsigned int JpegZigZagInputOrderCodes [JPEGSAMPLESIZE] ;
extern const unsigned int JpegZigZagOutputOrderCodes [JPEGSAMPLESIZE] ;

inline unsigned int JpegZigZagInputOrder (unsigned int item)
{
  ASSERT (item >= 0 && item <= JPEGSAMPLESIZE) ;
  return JpegZigZagInputOrderCodes [item] ;
}

inline unsigned int JpegZigZagOutputOrder (unsigned int item)
{
  ASSERT (item >= 0 && item <= JPEGSAMPLESIZE) ;
  return JpegZigZagOutputOrderCodes [item] ;
}

}
#endif
