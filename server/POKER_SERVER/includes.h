#ifndef __psi_includes_h__
#define __psi_includes_h__

#include "def.h"

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
#include <signal.h>
#include <stdlib.h>
#include <string.h>


// Include Debugging Defines
#include "debug.h"

// Include Global Defines
#include "global.h"

// Include Memory Management Routines
#include "pmemory.h"

// Include PDU Types
#include "pdutypes.h"

// XXX
#ifdef __cplusplus
#include "psi_assert.h"
#endif

#endif
