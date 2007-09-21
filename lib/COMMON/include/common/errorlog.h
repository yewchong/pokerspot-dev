#ifndef __cwg_errorlog_h__
#define __cwg_errorlog_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : CErrorLog
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style error logging.

 ****************************************************************************/


#include "common/log.h"


class CErrorLog
{
public:
  CErrorLog(const char* fname);
  virtual ~CErrorLog();

  static CLog& Log();

private:
  CLog log_;
  static CErrorLog* pInst_;
};

typedef CErrorLog Err;


#endif
