//
//  This file should not have a guard! It can be included multiple times.
//

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
//  JPEG Markers as defined in Section B.1.1.3 of the JPEG standard.
//


#ifndef JPEGMARKER
# define JPEGMARKER(a,b,c)
#endif

  JPEGMARKER (SOF0, 0xC0, "Start of Frame Baseline DCT")
, JPEGMARKER (SOF1, 0xC1, "Start of Frame Sequential DCT")
, JPEGMARKER (SOF2, 0xC2, "Start of Frame Progressive DCT")
, JPEGMARKER (SOF3, 0xC3, "Start of Frame Lossless")
  // Start of Frame Markers, Differential Huffman Coding
, JPEGMARKER (SOF5, 0xC5, "Start of Frame Differential Sequential DCT")
, JPEGMARKER (SOF6, 0xC6, "Start of Frame Differential Progressive DCT")
, JPEGMARKER (SOF7, 0xC7, "Start of Frame Differential Lossless")
  // Start of Frame Markers, Non-Differential Arithmetic Coding
, JPEGMARKER (SOF9, 0xC9, "Start of Frame Extended Sequential DCT Aritmetic")
, JPEGMARKER (SOFA, 0xCA, "Start of Frame Progressive DCT Aritmetic")
, JPEGMARKER (SOFB, 0xCB, "Start of Frame Lossless Aritmetic")
  // Start of Frame Markers, Differential Arithmetic Coding
, JPEGMARKER (SOFD, 0xCD, "Start of Frame Differential Sequential DCT Aritmetic")
, JPEGMARKER (SOFE, 0xCE, "Start of Frame Differential Progressive DCT Aritmetic")
, JPEGMARKER (SOFF, 0xCF, "Start of Frame Differential Lossless Aritmetic")
  // Other Markers
, JPEGMARKER (DHT, 0xC4,  "Define Huffman Tables")
, JPEGMARKER (DAC, 0xCC,  "Define Arithmetic Coding Conditions")
, JPEGMARKER (RST0, 0xD0, "Restart Marker 0")
, JPEGMARKER (RST1, 0xD1, "Restart Marker 1")
, JPEGMARKER (RST2, 0xD2, "Restart Marker 2")
, JPEGMARKER (RST3, 0xD3, "Restart Marker 3")
, JPEGMARKER (RST4, 0xD4, "Restart Marker 4")
, JPEGMARKER (RST5, 0xD5, "Restart Marker 5")
, JPEGMARKER (RST6, 0xD6, "Restart Marker 6")
, JPEGMARKER (RST7, 0xD7, "Restart Marker 7")
, JPEGMARKER (SOI, 0xD8,  "Start of Image")
, JPEGMARKER (EOI, 0xD9,  "End of Image")
, JPEGMARKER (SOS, 0xDA,  "Start of Scan")
, JPEGMARKER (DQT, 0xDB,  "Define Quantization Table")
, JPEGMARKER (DNL, 0xDC,  "Define Number of Lines")
, JPEGMARKER (DRI, 0xDD,  "Define Restart Intervale")
, JPEGMARKER (DHP, 0xDE,  "Define Hierarchical Progression")
, JPEGMARKER (EXP, 0xDF,  "Expand Reference Components")
, JPEGMARKER (APP0, 0xE0, "Application Segment 0")
, JPEGMARKER (APP1, 0xE1, "Application Segment 1")
, JPEGMARKER (APP2, 0xE2, "Application Segment 2")
, JPEGMARKER (APP3, 0xE3, "Application Segment 3")
, JPEGMARKER (APP4, 0xE4, "Application Segment 4")
, JPEGMARKER (APP5, 0xE5, "Application Segment 5")
, JPEGMARKER (APP6, 0xE6, "Application Segment 6")
, JPEGMARKER (APP7, 0xE7, "Application Segment 7")
, JPEGMARKER (APP8, 0xE8, "Application Segment 8")
, JPEGMARKER (APP9, 0xE9, "Application Segment 9")
, JPEGMARKER (APPA, 0xEA, "Application Segment A")
, JPEGMARKER (APPB, 0xEB, "Application Segment B")
, JPEGMARKER (APPC, 0xEC, "Application Segment C")
, JPEGMARKER (APPD, 0xED, "Application Segment D")
, JPEGMARKER (APPE, 0xEE, "Application Segment E")
, JPEGMARKER (APPF, 0xEF, "Application Segment F")
  // C8, F0-FD, 01, 02-BF reserved
, JPEGMARKER (SOFLS, 0xF7, "Start of Frame JPEG-LS") 
, JPEGMARKER (LSE, 0xF8, "JPEG-LS Extension") 
, JPEGMARKER (COM, 0xFE,  "Comment")
, JPEGMARKER (SOB, 0xFF,  "Start of Block")
#undef JPEGMARKER


