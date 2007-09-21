#ifndef __cwg_log_h__
#define __cwg_log_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : CLog
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style logging.

 ****************************************************************************/


#include "common/compat.h"
#include "common/filelogger.h"
#include <iostream.h>


class CLog
{
public:
  CLog(const char* fname, bool logToCerr = true);
  virtual ~CLog();

  CLog& operator<<(const char*);
  CLog& operator<<(char);
  CLog& operator<<(unsigned char);
  CLog& operator<<(int);
  CLog& operator<<(unsigned int);
  CLog& operator<<(long);
  CLog& operator<<(unsigned long);
  CLog& operator<<(float);
  CLog& operator<<(double);

  static char nl;

private:
  CFileLogger outfile_;
  bool logToCerr_;
};

inline CLog& CLog::operator<<(const char* p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(char p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(unsigned char p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(int p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(unsigned int p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(long p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(unsigned long p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(float p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}

inline CLog& CLog::operator<<(double p)
{
  cerr << p;
  outfile_ << p;
  return *this;
}


#endif

