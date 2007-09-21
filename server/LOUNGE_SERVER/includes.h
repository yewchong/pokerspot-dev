#ifndef __psi_includes_h__
#define __psi_includes_h__

#ifdef _WIN32
#include <windows.h>
#include <winsock.h>
#include <memory.h>
#include "psi_win32.h"
#else

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>          

typedef int SOCKET;

#endif 

// Common Libraries Between Win32 and *nix
#include <assert.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

// Include Debugging Defines
#include "debug.h"

// Include PDU Types
#include "common/pdutypes.h"

#define ASSERT(x) \
  { if (!(x)) { \
      string s; \
      char buf[34]; itoa(__LINE__, buf, 10); \
      s = string("*** Assertion Failed: expression: '") + string(#x) + \
          string("' file: ") + string(__FILE__) + string(" line: ") + \
          string(buf); \
      Sys_LogError("FATAL: assertion failed\n"); \
      Sys_LogError(s.c_str()); \
    } \
  }


#endif