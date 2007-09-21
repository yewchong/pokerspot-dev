#ifndef __table_gamemain_h__
#define __table_gamemain_h__

#include "includes.h"
#include "compat.h"

class Cplm;

struct ThreadInfo
{
  Cplm* plm_;
  bool  quit_;

  ThreadInfo()
    : plm_(NULL), quit_(false)
  {}
};

extern THREADPROC_RETURN CDECL GameMain(VOID* pV /* ThreadInfo* */);


#endif