#ifndef __cwg_filelogger_h__
#define __cwg_filelogger_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001
  Title    : CFileLogger
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style logging.

 ****************************************************************************/


#include <fstream.h>


class CFileLogger
{
public:
  CFileLogger(const char* fname);
  virtual ~CFileLogger();

  CFileLogger& operator<<(const char*);
  CFileLogger& operator<<(char);
  CFileLogger& operator<<(unsigned char);
  CFileLogger& operator<<(int);
  CFileLogger& operator<<(unsigned int);
  CFileLogger& operator<<(long);
  CFileLogger& operator<<(unsigned long);
  CFileLogger& operator<<(float);
  CFileLogger& operator<<(double);

private:
  ofstream outfile_;
};

inline CFileLogger& CFileLogger::operator<<(const char* p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(char p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(unsigned char p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(int p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(unsigned int p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(long p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(unsigned long p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(float p)
{
  outfile_ << p;
  return *this;
}

inline CFileLogger& CFileLogger::operator<<(double p)
{
  outfile_ << p;
  return *this;
}




#endif
