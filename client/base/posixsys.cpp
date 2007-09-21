#include "base/sysiface.h"
#include <pthread.h>

//
// Threads
//
int Sys_CreateThread(ThreadFunc pThreadFunc, void* pArgs)
{
  // XXX ok as stack variable?
  pthread_t thread;
  return pthread_create(&thread, NULL,
                        pThreadFunc,
                        pArgs);
}

void Sys_EndThread()
{
  pthread_exit(NULL);
}

//
// Event/Condition
// 
HANDLE Sys_CreateEvent()
{
  condition_bundle_t* pbundle = new condition_bundle_t;
  if (pbundle)
  {
    pbundle->condition_predicate = 0;
    pthread_mutex_init(&pbundle->condition_lock, NULL);
    pthread_cond_init(&pbundle->condition_variable, NULL);
  }
  return (HANDLE)pbundle;
}

int Sys_WaitEvent(HANDLE h, DWORD msecs)
{
  if (h == 0)
    return -1;

  time_t timeout;
  time(&timeout.tv_sec);
  timeout.tv_sec += (msecs / 1000);

  condition_bundle_t* pbundle = (condition_bundle_t*)h; 
  pthread_cond_timedwait(&pbundle->condition_variable,
                         &pbundle->contition_lock,
                         &timeout);

  return 1;
}

int Sys_SetEvent(HANDLE h)
{
  if (h == 0)
    return -1;

  condition_bundle_t* pbundle = (condition_bundle_t*)h;
  pthread_cond_signal(&pbundle->condition_variable);

  return 0;
}

int Sys_PulseEvent(HANDLE h)
{
  return 0;//::PulseEvent(h);
}

int Sys_ResetEvent(HANDLE h)
{
  return 0;//::ResetEvent(h);
}

void Sys_DestroyEvent(HANDLE h)
{
  pthread_cond_t* pcond = (pthread_cond_t*)h;
  pthread_cond_destroy(pcond);
  delete pcond;
   //::CloseHandle(h);
}


//
// Mutex
//
HANDLE Sys_CreateMutex()
{
  pthread_mutex_t* pmutex = new pthread_mutex_t;
  if (pmutex)
    pthread_mutex_init(pmutex, NULL);
  return (HANDLE)pmutex;
}

int Sys_LockMutex(HANDLE h, DWORD timeout)
{ // Locking a mutex does not support timeout!
  assert(timeout == INFINITE);
  if (timeout == INFINITE)
  {
    pthread_mutex_lock((pthread_mutex_t*)h);
    return 1;
  }
  else
    return 0;
}

int Sys_ReleaseMutex(HANDLE h)
{
  pthread_mutex_unlock((pthread_mutex_t*)h);
  return 0;
}

int Sys_DestroyMutex(HANDLE h)
{
  pthread_mutex_t* pmutex = (pthread_mutex_t*)h;
  pthread_mutex_destroy(pmutex);
  delete pmutex;
  return 0;
}


//
// Misc
//
void Sys_Sleep(DWORD msecs)
{
  sleep(msecs / 1000);
}


DWORD Sys_GetTicks()
{
  return 0;//time();
}

char* itoa(int i, char* buf, int base)
{
  sprintf(buf, "%i", i);
  return buf;
}