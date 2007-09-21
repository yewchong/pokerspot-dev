#ifndef __cwg_stringoutput_h__
#define __cwg_stringoutput_h__

/****************************************************************************

  Copyright (c) PSI Corp, 2001

  Title    : class CStringOutput
  Created  : 1/1/2001

  OVERVIEW : Class for doing C++ stream-style string output (instead of
             using sprintf).

 ****************************************************************************/

#ifdef PSPOT_TABLESERVER_MODULE_
#include <strstream>
#else
#include <strstrea.h>
#endif
#include "common/chips.h"

class CStringOutput
{
public:
  enum CSO_Const
  {
    Clear = 0
  };

  CStringOutput();
  virtual ~CStringOutput();

  CStringOutput& operator<<(const char*);
  CStringOutput& operator<<(char);
  CStringOutput& operator<<(unsigned char);
  CStringOutput& operator<<(int);
  CStringOutput& operator<<(unsigned int);
  CStringOutput& operator<<(long);
  CStringOutput& operator<<(unsigned long);
  CStringOutput& operator<<(float);
  CStringOutput& operator<<(double);
  CStringOutput& operator<<(const CChips&);
  CStringOutput& operator<<(CSO_Const);

  const char* str() const;
  void unfreeze(bool clear = true);
  void clear();

private:
#ifdef PSPOT_TABLESERVER_MODULE_
  std::ostrstream outstr_;
#else
  ostrstream outstr_;
#endif
  bool ends_;
};


inline CStringOutput& CStringOutput::operator<<(const char* p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(char p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(unsigned char p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(int p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(unsigned int p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(long p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(unsigned long p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(float p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(double p)
{
  outstr_ << p;
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(const CChips& chips)
{
  if (chips.getCents() != 0)
    outstr_ << '$' << chips.getDollars() << '.' << chips.getCents();
  else
    outstr_ << '$' << chips.getDollars();
  return *this;
}

inline CStringOutput& CStringOutput::operator<<(CSO_Const c)
{
  if (c == Clear)
    clear();
  return *this;
}

inline const char* CStringOutput::str() const
{
  if (!ends_)
  {
    const_cast<CStringOutput*>(this)->outstr_ << '\0';
    const_cast<CStringOutput*>(this)->ends_ = true;
  }
  return outstr_.rdbuf()->str();
}

inline void CStringOutput::unfreeze(bool clearit)
{
  outstr_.rdbuf()->freeze(0);
  if (clearit)
    clear();
}

inline void CStringOutput::clear()
{
  ends_ = false;

  // zero out the string
  if (outstr_.rdbuf()->str() != 0)
    *(outstr_.rdbuf()->str()) = '\0';

  outstr_.rdbuf()->freeze(0);
  outstr_.seekp(0);
}


// Shorthand
typedef CStringOutput CStrOut;

#endif