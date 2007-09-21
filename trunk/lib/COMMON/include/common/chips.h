#ifndef __chips_h__
#define __chips_h__

/****************************************************************************

  Copyright (c) Pokerspot 2001

  Title    : class CChips
  Created  : 4/9/2001

  OVERVIEW : Class for representing chips.

             This class uses a 32 bit unsigned integer for storing
             the chips of a player. 25 bits are used to represent the
             dollar amount and the remaining 7 bits are used for the
             cents. This gives a maximum of 32M dollars total.

 ****************************************************************************/


//#ifdef PSPOT_TABLESERVER_MODULE_
//#include <ostream>
//#endif

extern "C"
{
  #include <assert.h>
  #include <limits.h>
}


class CChips_n;



class CChips
{
public:
  enum {
    Fraction_Bits = 7,
    Digit_Bits    = 25,
    Max_Cents     = 99,
    Max_Dollars   = 33554432 // 32M
  };

  typedef unsigned long chip_amount_t;

public:
  CChips();
  CChips(double);
  CChips(const CChips&);
  CChips(chip_amount_t dollars, chip_amount_t cents);

  CChips& operator =  (const CChips&);
  CChips& operator += (const CChips&);
  CChips& operator -= (const CChips&);

  CChips& operator *= (int);
  CChips& operator /= (int);

  chip_amount_t getDollars() const;
  chip_amount_t getCents() const;
  chip_amount_t getRep() const;
  void setDollars(chip_amount_t);
  void setCents(chip_amount_t);
  void setRep(chip_amount_t);
  double asDouble() const;

  CChips_n htonl() const;
  chip_amount_t htonl_rep() const;

  friend CChips operator + (const CChips&, const CChips&);
  friend CChips operator + (chip_amount_t, const CChips&);
  friend CChips operator + (const CChips&, chip_amount_t);
  friend CChips operator - (const CChips&, const CChips&);
  friend CChips operator - (chip_amount_t, const CChips&);
  friend CChips operator - (const CChips&, chip_amount_t);
  friend CChips operator * (const CChips&, int);
  friend CChips operator * (int, const CChips&);
  friend CChips operator / (const CChips&, int);
  friend CChips operator % (const CChips&, int);

  friend bool operator == (const CChips&, const CChips&);
  friend bool operator != (const CChips&, const CChips&);
  friend bool operator <  (const CChips&, const CChips&);
  friend bool operator >  (const CChips&, const CChips&);
  friend bool operator <= (const CChips&, const CChips&);
  friend bool operator >= (const CChips&, const CChips&);
  
  friend bool operator == (const CChips&, chip_amount_t);
  friend bool operator != (const CChips&, chip_amount_t);
  friend bool operator <  (const CChips&, chip_amount_t);
  friend bool operator >  (const CChips&, chip_amount_t);
  friend bool operator <= (const CChips&, chip_amount_t);
  friend bool operator >= (const CChips&, chip_amount_t);
  
  friend bool operator == (chip_amount_t, const CChips&);
  friend bool operator != (chip_amount_t, const CChips&);
  friend bool operator <  (chip_amount_t, const CChips&);
  friend bool operator >  (chip_amount_t, const CChips&);
  friend bool operator <= (chip_amount_t, const CChips&);
  friend bool operator >= (chip_amount_t, const CChips&); 


private:
  chip_amount_t rep_;
};


// member functions

inline CChips& CChips::operator=(const CChips& rhs)
{
  if (this != &rhs)
  {
    rep_ = rhs.rep_;
  }
  return *this;
}

inline CChips& CChips::operator += (const CChips& rhs)
{
  *this = *this + rhs;
  return *this;
}

inline CChips& CChips::operator -= (const CChips& rhs)
{
  *this = *this - rhs;
  return *this;
}

inline CChips& CChips::operator *= (int d)
{
  *this = (*this) * d;
  return *this;
}

inline CChips& CChips::operator /= (int d)
{
  assert(d > 0);
  *this = (*this) / d;
  return *this;
}

inline CChips::chip_amount_t CChips::getDollars() const
{
  return rep_ >> Fraction_Bits;
}

inline CChips::chip_amount_t CChips::getCents() const
{
  // Mask off all but 7 rightmost bits
  return rep_ & 127;
}

inline CChips::chip_amount_t CChips::getRep() const
{
  return rep_;
}

inline void CChips::setDollars(CChips::chip_amount_t dollars)
{
  assert(dollars <= Max_Dollars);
  dollars <<= Fraction_Bits;
  chip_amount_t cents = getCents();
  dollars |= cents;
  rep_ = dollars;
}

inline void CChips::setCents(CChips::chip_amount_t cents)
{
  assert(cents <= Max_Cents);
  // mask off old cents
  rep_ &= (ULONG_MAX - 127);
  // add new cents
  rep_ |= cents;
}

inline double CChips::asDouble() const
{
  double d = getDollars();
  d += ((double)getCents() / 100.0);
  return d;
}

inline void CChips::setRep(CChips::chip_amount_t r)
{
  rep_ = r;
}


// free functions

inline CChips operator + (CChips::chip_amount_t d, const CChips& rhs)
{
  return CChips(d, 0) + rhs;
}

inline CChips operator + (const CChips& lhs, CChips::chip_amount_t d)
{
  return lhs + CChips(d, 0);
}

inline CChips operator - (CChips::chip_amount_t d, const CChips& rhs)
{
  return CChips(d, 0) - rhs;
}

inline CChips operator - (const CChips& lhs, CChips::chip_amount_t d)
{
  return lhs - CChips(d, 0);
}

inline CChips operator * (int d, const CChips& rhs)
{
  return rhs * d;
}


inline bool operator == (const CChips& lhs, const CChips& rhs)
{
  return lhs.rep_ == rhs.rep_;
}

inline bool operator != (const CChips& lhs, const CChips& rhs)
{
  return lhs.rep_ != rhs.rep_;
}

inline bool operator <  (const CChips& lhs, const CChips& rhs)
{
  return lhs.rep_ < rhs.rep_;
}

inline bool operator >  (const CChips& lhs, const CChips& rhs)
{
  return lhs.rep_ > rhs.rep_;
}

inline bool operator <= (const CChips& lhs, const CChips& rhs)
{
  return lhs.rep_ <= rhs.rep_;
}

inline bool operator >= (const CChips& lhs, const CChips& rhs)
{
  return lhs.rep_ >= rhs.rep_;
}


inline bool operator == (const CChips& lhs, CChips::chip_amount_t dollars)
{
  return lhs == CChips(dollars, 0);
}

inline bool operator != (const CChips& lhs, CChips::chip_amount_t dollars)
{
  return !(lhs == dollars);
}

inline bool operator < (const CChips& lhs, CChips::chip_amount_t dollars)
{
  return lhs.getDollars() < dollars;
}

inline bool operator > (const CChips& lhs, CChips::chip_amount_t dollars)
{
  return lhs > CChips(dollars, 0);
}

inline bool operator <= (const CChips& lhs, CChips::chip_amount_t dollars)
{
  return lhs <= CChips(dollars, 0);
}

inline bool operator >= (const CChips& lhs, CChips::chip_amount_t dollars)
{
  return lhs >= CChips(dollars, 0);
}



inline bool operator == (CChips::chip_amount_t dollars, const CChips& rhs)
{
  return CChips(dollars, 0) == rhs;
}

inline bool operator != (CChips::chip_amount_t dollars, const CChips& rhs)
{
  return !(dollars == rhs);
}

inline bool operator <  (CChips::chip_amount_t dollars, const CChips& rhs)
{
  return CChips(dollars, 0) < rhs;
}

inline bool operator >  (CChips::chip_amount_t dollars, const CChips& rhs)
{
  return CChips(dollars, 0) > rhs;
}

inline bool operator <= (CChips::chip_amount_t dollars, const CChips& rhs)
{
  return CChips(dollars, 0) <= rhs;
}

inline bool operator >= (CChips::chip_amount_t dollars, const CChips& rhs)
{
  return CChips(dollars, 0) >= rhs;
}

inline CChips minimum(const CChips& lhs, const CChips& rhs)
{
  if (lhs <= rhs)
  {
    return lhs;
  }
  else
  {
    return rhs;
  }
}

inline CChips maximum(const CChips& lhs, const CChips& rhs)
{
  if (lhs > rhs)
  {
    return lhs;
  }
  else
  {
    return rhs;
  }
}


/*
#ifdef PSPOT_TABLESERVER_MODULE_
inline std::ostream& operator << (std::ostream& out, const CChips& rhs)
{
  if (rhs.getCents() != 0)
    out << rhs.getDollars() << '.' << rhs.getCents();
  else
    out << rhs.getDollars();
  return out;
}
#endif
*/

// This chip class is used for transferring the
// chip quantities over network. The quantity is
// stored in network byte order.
class CChips_n
{
public:

  CChips_n(CChips::chip_amount_t);

  CChips ntohl() const;

  CChips::chip_amount_t rep_n;
};




#endif
