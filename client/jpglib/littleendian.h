#ifndef CB__LITTLEENDIAN_H
#define CB__LITTLEENDIAN_H
//
//  Title:  Little Endian Byte Conversion Functions
//
//  Copyright 1999, Colosseum Builders, Inc.
//  All rights reserved.
//
//  Colosseum Builders, Inc. makes no warranty, expressed or implied
//  with regards to this software. It is provided as is.
//
//  Author:  John M. Miano (miano@colosseumbuilders.com)
//
//  Date:    March 15, 1999
//
//  Version: 1
//
//  Description:
//
//    Byte conversion functions for little endian systems.
//

inline UBYTE2 SystemToLittleEndian (UBYTE2 value)
{
  return value ;
}
inline UBYTE4 SystemToLittleEndian (UBYTE4 value)
{
  return value ;
}

inline BYTE4 SystemToLittleEndian (BYTE4 value)
{
  return value ;
}


inline UBYTE2 SystemToBigEndian (UBYTE2 value)
{
  return (UBYTE2) ((value&0xFF) << 8)|((value&0xFF00)>>8) ;
}
inline UBYTE4 SystemToBigEndian (UBYTE4 value)
{
  return ((value&0xFF000000L)>>24)|((value&0xFF0000L)>>8)
         | ((value&0xFF00L)<<8) | ((value&0xFFL)<<24) ;
}

inline UBYTE2 LittleEndianToSystem (UBYTE2 value)
{
  return value ;
}
inline UBYTE4 LittleEndianToSystem (UBYTE4 value)
{
  return value ;
}
inline UBYTE2 BigEndianToSystem (UBYTE2 value)
{
  return (UBYTE2) ((value&0xFF) << 8)|((value&0xFF00)>>8) ;
}
inline UBYTE4 BigEndianToSystem (UBYTE4 value)
{
  return ((value&0xFF000000L)>>24)|((value&0xFF0000L)>>8)
         | ((value&0xFF00L)<<8) | ((value&0xFFL)<<24) ;
}

#endif
