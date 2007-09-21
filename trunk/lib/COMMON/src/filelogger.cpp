/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : logger.h
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style logging.

 ****************************************************************************/

#include "common/filelogger.h"
#include <ios.h>

CFileLogger::CFileLogger(const char* fname)
  :
  outfile_(fname, ios::app)
{}


CFileLogger::~CFileLogger()
{}

