/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class SyncObject, class Event, class Mutex, class SingleLock
  Created  : 15.11.1999

  OVERVIEW : Classes for synchronizing threads.

 ****************************************************************************/


#include "stdafx.h"
#include "base/sync.h"

BEGIN_NAMESPACE(Base)

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
  assert(hObject_ == NULL);

  /* Subclass must deallocate
  if (hObject_ != NULL)
  {
    ::CloseHandle(hObject_);
    hObject_ = NULL;
  }
  */
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
    // error out of resources!
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
  return Sys_LockMutex(hObject_, dwTimeout);
}

BOOL Mutex::unlock()
{
  return Sys_ReleaseMutex(hObject_);
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
    // error out of resources!
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


END_NAMESPACE(Base)