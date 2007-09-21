#include "stdafx.h"
#include "base/sysiface.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


// Link in the multimedia library (for timeGetTime)
#pragma comment (lib, "winmm.lib")

//
// Threads
//
int Sys_CreateThread(ThreadFunc pThreadFunc, void* pArgs)
{
  return ::_beginthread(pThreadFunc, 0, pArgs);
}

void Sys_EndThread()
{
  ::_endthread();
}

//
// Event/Condition
// 
HANDLE Sys_CreateEvent()
{
  return ::CreateEvent(NULL, FALSE, FALSE, NULL);
}

int Sys_WaitEvent(HANDLE h, DWORD dwTimeout)
{
  // WIN32 has same implementation for events & mutexes
  return Sys_LockMutex(h, dwTimeout);
}

int Sys_SetEvent(HANDLE h)
{
  return ::SetEvent(h);
}

int Sys_PulseEvent(HANDLE h)
{
  return ::PulseEvent(h);
}

int Sys_ResetEvent(HANDLE h)
{
  return ::ResetEvent(h);
}

void Sys_DestroyEvent(HANDLE h)
{
  ::CloseHandle(h);
}


//
// Mutex
//
HANDLE Sys_CreateMutex()
{
  return ::CreateMutex(NULL, FALSE, NULL);
}

int Sys_LockMutex(HANDLE h, DWORD timeout)
{
  if (::WaitForSingleObject(h, timeout) == WAIT_OBJECT_0)
    return 1;
  else
    return 0;
}

int Sys_ReleaseMutex(HANDLE h)
{
  return ::ReleaseMutex(h);
}

int Sys_DestroyMutex(HANDLE h)
{
  return ::CloseHandle(h);
}


//
// Misc
//
void Sys_Sleep(DWORD msecs)
{
  ::Sleep(msecs);
}

DWORD Sys_GetTicks()
{
  return ::timeGetTime();
}