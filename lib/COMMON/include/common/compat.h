#ifndef __cwg_compat_h__
#define __cwg_compat_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : compat.h
  Created  : 09/21/2001
 
  OVERVIEW : Header file for making the source compile on
             both Win32 and *nix.

 ****************************************************************************/

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#endif


#ifdef _WIN32
typedef int socklen_t;
#define MAXHOSTNAMELEN (256)

#define USING_NS_T(ns, t) \
  using ns::t;
#define USING_NS(ns) \
  using namespace ns;
#endif

#ifdef _WIN32
typedef unsigned char  u_byte_t;
typedef unsigned char  u_int8_t;
typedef unsigned short u_int16_t;
typedef unsigned int   u_int32_t;
#endif

#ifdef _WIN32
// Disable harmless compiler warning C4786 as many STL template
// classes generate this:
// "identifier was truncated to '255' characters in the debug information"
#pragma warning( disable : 4786 )
#endif


#endif