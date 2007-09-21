
//
//  Title:  Cyclic Reducancy Check Template Class Implementation
//          Hand Instanciated for MSVC++
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
//    This is a template class for implementing various forms of the
//    CRC function.
//
//  Revision History:
//
//

#include "crc32.h"

const int BITSPERBYTE = 8 ;
const UBYTE4 bits [BITSPERBYTE * 4] =
 { 0x00000001UL, 0x00000002UL, 0x00000004UL, 0x00000008UL,
   0x00000010UL, 0x00000020UL, 0x00000040UL, 0x00000080UL,
   0x00000100UL, 0x00000200UL, 0x00000400UL, 0x00000800UL,
   0x00001000UL, 0x00002000UL, 0x00004000UL, 0x00008000UL,
   0x00010000UL, 0x00020000UL, 0x00040000UL, 0x00080000UL,
   0x00100000UL, 0x00200000UL, 0x00400000UL, 0x00800000UL,
   0x01000000UL, 0x02000000UL, 0x04000000UL, 0x08000000UL,
   0x10000000UL, 0x20000000UL, 0x40000000UL, 0x80000000UL,
 } ;

namespace
{
//
//  Description:
//
//    This function returns the reversed bit patter from its input.
//    For example, 1010 becomes 0101.
//
//  Parameters:
//
//    value: The value to reverse
//    bitcount: The number of bits to reverse.
//
BYTE4 Reverse (UBYTE4 value, unsigned int bitcount)
{
  unsigned long result = 0 ;

  for (unsigned int jj = 0 ; jj < bitcount ; ++ jj)
  {
    if ((value & bits [jj]) != 0)
      result |= bits [bitcount - jj - 1] ;
  }
  return result ;
}


//
//  Description:
//
//    This function creates a CRC table entry for a reversed
//    CRC function.
//
//  Parameters:
//
//      polynomial: The CRC Polynomial.
//      entryindex: The index of the CRC table entry.
//      bitcount:  The number of bits per CRC table entry.
//
//  Return Value:
//
//      The value for the specified CRC table entry.
//
UBYTE4 ReverseTableEntry (UBYTE4 polynomial,
                          unsigned int entryindex,
                          unsigned int bitcount)
{
  UBYTE4 result = entryindex ;
  for (unsigned int ii = 0 ; ii < BITSPERBYTE ; ++ ii)
  {
    if ((result & 1) == 0)
      result >>= 1 ;
    else
      result = (result >> 1) ^ Reverse (polynomial, bitcount) ;
  }
  unsigned long mask = ((1UL << (bitcount - 1)) - 1)
                     | (1UL << (bitcount - 1)) ;
  result &=  mask ;
  return result ;
}

} // END unnamed namespace

namespace Colosseum
{
//
// Definition of the Crc class's static member variable.
//
const Crc32::CrcTable Crc32::crc_table ;

//
//  Description:
//
//    Default Constructor
//
Crc32::Crc32 ()
{
  reset () ;
  return ;
}

//
//  Description:
//
//    Copy Constructor
//
Crc32::Crc32 (const Crc32 &source)
: crc_register (source.crc_register)
{
  return ;
}

//
//  Description:
//
//    Assignment Operator
//
Crc32 &Crc32::operator=(const Crc32 &source)
{
  crc_register = source.crc_register ;
  return *this ;
}

//
//  Description:
//
//    This function resets the CRC register to it initial state.  Call
//    this member function after you have calculated a CRC value and
//    want to calculate another.
//
void Crc32::reset ()
{
  crc_register = 0xFFFFFFFF ;
  return ;
}

//
//  Description:
//
//    This function retrieves the value of the CRC register. Some forms of the
//    CRC use an XOR operation
//
//  Return Value:
//
//    The current CRC value.
//
UBYTE4 Crc32::value () const
{
  UBYTE4 result = crc_register ^ 0xFFFFFFFF ;
  return result ;
}

//
//  Description:
//
//    This function updates the value of the CRC register based upon
//    the contents of a buffer.
//
//  Parameters:
//
//    buffer: The input buffer
//    length: The length of the input buffer.
//
void Crc32::update (const char *buffer, unsigned int length)
{
  for (unsigned int ii = 0 ; ii < length ; ++ ii)
  {
    crc_register = crc_table.values [(crc_register ^ buffer [ii]) & 0xFF]
                 ^ (crc_register >> 8) ;
  }
  return ;
}

//
//  Description:
//
//    CrcTable Class Constructor
//
//    The CRC table is implemented as a separate class to that
//    CRC tables can be static members, initialized at program
//    startup.
//
//
Crc32::CrcTable::CrcTable ()
{
  for (unsigned int ii = 0 ; ii < MAXBYTEVALUES ; ++ ii)
  {
    values [ii] = ReverseTableEntry (0x04C11DB7, ii, 32) ;
  }
}

}

