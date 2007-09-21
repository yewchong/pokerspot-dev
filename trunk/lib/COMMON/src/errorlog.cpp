/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : CLog
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style error logging.

 ****************************************************************************/


#include "common/errorlog.h"
#include "common/sys.h"


CErrorLog* CErrorLog::pInst_ = 0;


CErrorLog::CErrorLog(const char* fname)
  :
  log_(fname)
{
  pInst_ = this;
}


CErrorLog::~CErrorLog()
{
  pInst_ = NULL;
}


CLog& CErrorLog::Log()
{
  pInst_->log_ << Sys_GetDate();
  return pInst_->log_;
}

#ifdef ERRORLOG_MODULE_TEST_

int main(int, const char**)
{
  CErrorLog e("errorlog.txt");

  err::Log() << "Testing testing 1 2 3";

  return 0;
}

#endif