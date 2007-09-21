/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class SyncObject, class Event, class Mutex, class SingleLock
  Created  : 04/10/2000

  OVERVIEW : Classes for synchronizing threads.

 ****************************************************************************/

#include "sync.h"

//-------------------------------------------------------------------
//
// SyncObject
//
SyncObject::SyncObject()
  :
  hObject_(NULL)
{
}


SyncObject::~SyncObject()
{
  // Subclass must deallocate
}

#define MAX_MUTEXES 100
int g_DebugMutex[MAX_MUTEXES];
int g_MutexCounter = -1;

//-------------------------------------------------------------------
//
// Mutex
//
Mutex::Mutex()
{
  if (g_MutexCounter == -1)
  {
      for (int j = 0; j < MAX_MUTEXES; j++)
      {
          g_DebugMutex[j] = -1;
      }
  }

  if (g_MutexCounter < MAX_MUTEXES)
  {
    counter_ = (g_MutexCounter + 1);
    g_MutexCounter++;
  }


  hObject_ = Sys_CreateMutex();
  if (hObject_ == NULL)
  {
    Sys_LogError("Mutex::Mutex: failed to create mutex");
  }
}


Mutex::~Mutex()
{
  if (hObject_ != NULL)
  {
    Sys_DestroyMutex(hObject_);
    hObject_ = NULL;
  }
}


BOOL Mutex::lock(DWORD dwTimeout)
{
  g_DebugMutex[counter_] = 2;
  BOOL rc = Sys_LockMutex(hObject_, dwTimeout);
  g_DebugMutex[counter_] = 3;
  return rc;
}

BOOL Mutex::unlock()
{
  g_DebugMutex[counter_] = 1;
  BOOL rc = Sys_ReleaseMutex(hObject_);
  g_DebugMutex[counter_] = 0;
  return rc;
}



//-------------------------------------------------------------------
//
// Event
//
Event::Event()
{
  hObject_ = Sys_CreateEvent();
  if (hObject_ == NULL)
  {
    Sys_LogError("Event::Event: failed to create event");
  }
}


Event::~Event()
{
  if (hObject_)
  {
    Sys_DestroyEvent(hObject_);
    hObject_ = NULL;
  }
}

BOOL Event::lock(DWORD dwTimeout)
{
  return Sys_WaitEvent(hObject_, dwTimeout);
}

BOOL Event::unlock()
{
  return TRUE;
}