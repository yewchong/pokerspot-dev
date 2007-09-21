#ifndef __cwg_sys_h__
#define __cwg_sys_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : sys.h
  Created  : 09/21/2000

  OVERVIEW : Collection of miscellaneous functions.

 ****************************************************************************/

#include <windows.h>
#include <winsock.h>

// Socket functions

extern int Sys_InitSocket     ();
extern int Sys_ShutdownSocket ();

extern int Sys_closesocket    (SOCKET);
extern int Sys_accept         (SOCKET listenfd,
                               struct sockaddr* cliaddr,
                               int* addrlen);
extern int Sys_peek           (SOCKET, char* buffer, int nbytes);
extern int Sys_recv           (SOCKET, char* buffer, int nbytes);
extern int Sys_send           (SOCKET, const char* buffer, int nbytes);
extern int Sys_setnonblockio  (SOCKET);
extern int Sys_setnonagle     (SOCKET);
extern int Sys_ErrIsWouldBlock();
extern int Sys_accept_SSL     (SOCKET);

extern SOCKET MakeConnection  (UINT ipaddr, short port);
extern BOOL Sys_SSLEnabled();
extern void Sys_SetSSLEnabled(BOOL);

// Thread creation/ending 
#define THREADPROC_RETURN void
typedef void (__cdecl *ThreadFunc)(void*);
extern int  Sys_CreateThread(ThreadFunc, void* args);
extern void Sys_EndThread();

// Event/Condition
extern HANDLE Sys_CreateEvent  ();
extern int    Sys_SetEvent     (HANDLE);
extern int    Sys_ResetEvent   (HANDLE);
extern int    Sys_PulseEvent   (HANDLE);
extern int    Sys_WaitEvent    (HANDLE, DWORD msecs);
extern void   Sys_DestroyEvent (HANDLE);

// Mutex
extern HANDLE Sys_CreateMutex  ();
extern int    Sys_LockMutex    (HANDLE, DWORD msecs);
extern int    Sys_ReleaseMutex (HANDLE);
extern int    Sys_DestroyMutex (HANDLE);

// Misc
extern DWORD       Sys_GetLastError();
extern DWORD       Sys_GetTicks    ();
extern void        Sys_Sleep       (DWORD msecs);
extern const char* Sys_GetDate     ();
extern DWORD       Sys_GetLastSocketError();


#endif