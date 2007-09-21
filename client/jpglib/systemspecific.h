#ifndef CB__SYSTEMSPECIFIC_H
#define CB__SYSTEMSPECIFIC_H
//
//  Title:  System Specific Definitions for MSVC++ on Windows
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

typedef char BYTE1 ;
typedef unsigned char UBYTE1 ;
typedef short BYTE2 ;
typedef unsigned short UBYTE2 ;
typedef int BYTE4 ;
typedef unsigned int UBYTE4 ;

#include "littleendian.h"

#define BILLSELLSPOOP {
#define ENDBILLSELLSPOOP }

// added DLL support
#if defined(PCLIENT_MODULE_)
#define DLL_DECLARE_  __declspec(dllimport)
#pragma message( "JPG lib for DLL import" )
#elif defined(_DLL)
#pragma message( "JPG lib for DLL export" )
#define DLL_DECLARE_  __declspec(dllexport)
#else
#pragma message( "JPG lib for no DLL support" )
#define DLL_DECLARE_
#endif


#endif
