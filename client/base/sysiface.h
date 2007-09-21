#ifndef __base_sysiface_h__
#define __base_sysiface_h__

//
// Windows/Unix system functions
//

#include "network/compat.h"

#ifdef _WIN32
  typedef void (__cdecl *ThreadFunc)(void*);
#else
  typedef void* (*ThreadFunc)(void*);
#endif

// Thread creation/ending 
extern 
int Sys_CreateThread(ThreadFunc, void* args);
extern
void Sys_EndThread();

// Event/Condition
extern
HANDLE Sys_CreateEvent();
extern
int Sys_SetEvent(HANDLE);
extern
int Sys_ResetEvent(HANDLE);
extern
int Sys_PulseEvent(HANDLE);
extern
int Sys_WaitEvent(HANDLE, DWORD msecs);
extern
void Sys_DestroyEvent(HANDLE);

// Mutex
extern
HANDLE Sys_CreateMutex();
extern
int Sys_LockMutex(HANDLE, DWORD msecs);
extern
int Sys_ReleaseMutex(HANDLE);
extern
int Sys_DestroyMutex(HANDLE);

// Misc
extern
DWORD Sys_GetTicks();
extern
void Sys_Sleep(DWORD msecs);
extern
void Sys_OutputDebugString(const char*);

#endif