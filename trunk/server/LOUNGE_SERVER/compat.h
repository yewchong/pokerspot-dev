#ifndef __psi_compat_h__
#define __psi_compat_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : compat.h
  Created  : 1/9/2000

  OVERVIEW : Unix/Windows compatibility header.

 ****************************************************************************/

#ifdef _WIN32
// Will use WIN32 native threads on Windows
//#define _POSIX_THREADS 1
#endif

#if defined(_POSIX_THREADS)
    #define THREADPROC_RETURN void*
    typedef void* (*ThreadFunc)(void*);
#elif defined(_WIN32)
    #define THREADPROC_RETURN void
    typedef void (__cdecl *ThreadFunc)(void*);
#else
    // assume POSIX if not win32
    #define THREADPROC_RETURN void*
    typedef void* (*ThreadFunc)(void*);
#endif




#ifdef _WIN32

//
// MS-Windows
//
#include <winsock.h>
#include <process.h>
#include <mmsystem.h>

// Those structures that are sent over network will
// be aligned on two byte boundaries.
#define NETWORK_ALIGN_BEGIN pack (push, 2)
#define NETWORK_ALIGN_END pack (pop)


#define BEGIN_NAMESPACE(x) \
  namespace x {
#define END_NAMESPACE(x) \
  }

#define USING_NS_T(ns, t) \
	using ns::t;
#define USING_NS(ns)

#define BEGIN_NAMESPACE0() \
  namespace {
#define END_NAMESPACE0() \
  }


#define WSA_ERROR(x) \
  { printf("Error %d: %s\n", \
      WSAGetLastError(), x); }


#define MAXHOSTNAMELEN 256
typedef int socklen_t;


#else

//
// POSIX
//
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>

typedef int SOCKET;
typedef void VOID;
typedef void* LPVOID;
typedef int HANDLE;
typedef int HWND;
typedef int BOOL;
typedef unsigned int UINT;
typedef unsigned short USHORT;
typedef unsigned long DWORD;
typedef const char* LPCSTR;
#define INVALID_SOCKET (-1)
#define SOCKET_ERROR (-1)
#define FALSE 0
#define TRUE (!FALSE)
#define INFINITE 0xffff

// gcc does not support namespaces, define
// them empty
#define BEGIN_NAMESPACE(x)
#define END_NAMESPACE(x)  
#define USING_NS_T(ns, t)
#define USING_NS(ns)
#define BEGIN_NAMESPACE0()
#define END_NAMESPACE0()

#define WSA_ERROR(x) \
  { perror((x)); }
#define CDECL 
#define closesocket(sd) close((sd))
#define lstrlen(x) strlen((x))
#define OutputDebugString(x) printf(x)

extern char* itoa(int, char*, int);


#endif // !_WIN32

extern "C" int InitSockets(void);

#endif
