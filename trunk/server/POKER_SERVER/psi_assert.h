#ifndef __psi_assert_h__
#define __psi_assert_h__

#include "compat.h"
#include <string>
USING_NS_T(std, string);

// This assert macro is to be used to assert fatal conditions.
// If the condition fails, the server will send a message to the
// players, save state to shutdownlog-x.txt, and shutdown itself.

extern void DoShutdown(const char* msg);

#ifdef _WIN32
    extern int psi_new_handler(size_t);
#else
    extern void psi_new_handler();
#endif


#define ASSERT(x) \
  { if (!(x)) { \
      string s; \
      char buf[34]; itoa(__LINE__, buf, 10); \
      s = string("*** Assertion Failed: expression: '") + string(#x) + \
          string("' file: ") + string(__FILE__) + string(" line: ") + \
          string(buf); \
      Sys_LogError("FATAL: assertion failed\n"); \
      Sys_LogError(s.c_str()); \
      assert(x); \
    } \
  }


/*
#define ASSERT(x) \
  { if (!(x)) { \
      string s; \
      char buf[34]; itoa(__LINE__, buf, 10); \
      s = string("*** Assertion Failed: expression: '") + string(#x) + \
          string("' file: ") + string(__FILE__) + string(" line: ") + \
          string(buf); \
      DoShutdown(s.c_str()); \
    } \
  }
*/

struct FatalRuntimeError
{
    FatalRuntimeError() {}
    ~FatalRuntimeError() {}
};

#endif