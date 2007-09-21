/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : CLog
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style logging.

 ****************************************************************************/


#include "common/log.h"

char CLog::nl = '\n';

CLog::CLog(const char* fname, bool logToCerr)
  :
  outfile_(fname),
  logToCerr_(logToCerr)
{}


CLog::~CLog()
{}
