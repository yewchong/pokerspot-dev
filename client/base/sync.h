#ifndef __base_sync_h__
#define __base_sync_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class SyncObject, class Event, class Mutex, class SingleLock
  Created  : 15.11.1999

  OVERVIEW : Classes for synchronizing threads.

 ****************************************************************************/


#ifdef _WIN32
  #include <winbase.h>
#else
  #define INFINITE 0xffff
#endif
#include <assert.h>
#include "base/sysiface.h"

BEGIN_NAMESPACE(Base)


//-------------------------------------------------------------------
//
// SyncObject base class
//
class SyncObject
{
public:
  operator HANDLE() const { return hObject_; }  
  HANDLE   handle() const { return hObject_; }

  virtual BOOL lock(DWORD dwTimeout = INFINITE) = 0;
  virtual BOOL unlock() = 0;

  SyncObject();
  virtual ~SyncObject();

protected:
  HANDLE  hObject_;
};


//-------------------------------------------------------------------
//
// Event
//
class Event : public SyncObject
{
public:
  virtual BOOL lock(DWORD dwTimeout = INFINITE);
  virtual BOOL unlock();

  BOOL setEvent();
  BOOL pulseEvent();
  BOOL resetEvent();

  Event();
  virtual ~Event();
};

inline BOOL Event::setEvent()
{ 
  assert(hObject_ != 0);
  return Sys_SetEvent(hObject_);
}

inline BOOL Event::pulseEvent()
{ 
  assert(hObject_ != 0);
  return Sys_PulseEvent(hObject_);
}

inline BOOL Event::resetEvent()
{
  assert(hObject_ != 0);
  return Sys_ResetEvent(hObject_);
}



//-------------------------------------------------------------------
//
// Mutex
//
class Mutex : public SyncObject
{
public:
  virtual BOOL lock(DWORD dwTimeout = INFINITE);
  virtual BOOL unlock();

  Mutex();
  virtual ~Mutex();
};



//-------------------------------------------------------------------
//
// SingleLock
//
class SingleLock
{
public:
  SingleLock(SyncObject* pSO)
    :
    pSyncObject_(pSO)
  {}

  BOOL lock(DWORD dwTimeout = INFINITE)
  {
    return pSyncObject_->lock(dwTimeout);
  }
  
  ~SingleLock()
  {
    pSyncObject_->unlock();
  }

private:
  SyncObject* pSyncObject_;
};


END_NAMESPACE(Base)


#endif