#ifndef __cwg_sync_h__
#define __cwg_sync_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class SyncObject, class Event, class Mutex, class SingleLock
  Created  : 04/10/2000

  OVERVIEW : Classes for synchronizing threads.

 ****************************************************************************/


#ifdef _WIN32
  #include <windows.h>
  #include <winbase.h>
#else
  #define INFINITE 0xffff
#endif
#include <assert.h>
#include "common/sys.h"


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

private:
    int counter_;
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


#endif