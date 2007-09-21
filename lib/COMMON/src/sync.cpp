/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class SyncObject, class Event, class Mutex, class SingleLock
  Created  : 04/10/2000

  OVERVIEW : Classes for synchronizing threads.

 ****************************************************************************/

#include "common/sync.h"


//-------------------------------------------------------------------
//
// SyncObject
//
SyncObject::SyncObject()
  :
  hObject_(NULL)
{}


SyncObject::~SyncObject()
{
  // Subclass must deallocate
}

//-------------------------------------------------------------------
//
// Mutex
//
Mutex::Mutex()
{
  hObject_ = Sys_CreateMutex();
  if (hObject_ == NULL)
  {
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
  BOOL rc = Sys_LockMutex(hObject_, dwTimeout);
  return rc;
}

BOOL Mutex::unlock()
{
  BOOL rc = Sys_ReleaseMutex(hObject_);
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