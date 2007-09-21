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


#include "common/chips.h"
#include <winsock2.h>
#include <math.h>

namespace
{

double RoundDouble(double doValue, int nPrecision)
{
	static const double doBase = 10.0;
	double doComplete5, doComplete5i;
	
	doComplete5 = doValue * pow(doBase, (double)(nPrecision + 1));
	
	if (doValue < 0.0)
		doComplete5 -= 5.0;
	else
		doComplete5 += 5.0;
	
	doComplete5 /= doBase;
	modf(doComplete5, &doComplete5i);
	
	return doComplete5i / pow(doBase, (double) nPrecision);
}

}


CChips::CChips()
  :
  rep_(0)
{}

CChips::CChips(const CChips& rhs)
  :
  rep_(rhs.rep_)
{}

CChips::CChips(double d)
  :
  rep_(0)
{
  assert(d <= Max_Dollars);
  chip_amount_t dollars = (chip_amount_t)d;
  setDollars(dollars);

  // Extract the 2 decimals
  double tmp = 100.0 * (d - (double)dollars);
  chip_amount_t cents = (chip_amount_t)RoundDouble(tmp, 0);
  assert(cents <= Max_Cents);
  setCents(cents);
}

  
CChips::CChips(CChips::chip_amount_t dollars, CChips::chip_amount_t cents)
  :
  rep_(0)
{
  setDollars(dollars);
  setCents(cents);
}

CChips_n CChips::htonl() const
{
  chip_amount_t rep = ::htonl(rep_);  
  return CChips_n(rep);
}

CChips::chip_amount_t CChips::htonl_rep() const
{
  return ::htonl(rep_);  
}

CChips operator + (const CChips& lhs, const CChips& rhs)
{
  CChips::chip_amount_t dollars = lhs.getDollars() + rhs.getDollars();
  assert(dollars <= CChips::Max_Dollars);

  CChips::chip_amount_t cents = lhs.getCents() + rhs.getCents();
  if (cents >= 100)
  {
    dollars += 1;
    cents -= 100;
  }

  return CChips(dollars, cents);
}


CChips operator - (const CChips& lhs, const CChips& rhs)
{
  assert(lhs >= rhs);

  CChips::chip_amount_t dollars = lhs.getDollars() - rhs.getDollars();
  assert(dollars <= CChips::Max_Dollars);

  int cents = lhs.getCents() - rhs.getCents();
  assert(cents <= 100);

  if (cents < 0)
  {
    dollars -= 1;
    cents = 100 + cents;
  }

  return CChips(dollars, cents);
}

CChips operator * (const CChips& lhs, int d)
{
  CChips::chip_amount_t dollars = d * lhs.getDollars();
  assert(dollars <= CChips::Max_Dollars);

  CChips::chip_amount_t cents = d * lhs.getCents();
  while (cents >= 100)
  {
    dollars += 1;
    cents -= 100;
  }

  return CChips(dollars, cents);
}

CChips operator / (const CChips& lhs, int d)
{
#ifdef _DEBUG
  assert(d > 0);
#endif

  CChips chips;

  if (d > 0)
  {
    chips.setDollars(lhs.getDollars() / d);
    chips.setCents(lhs.getCents() / d);
  }

  return chips;
}



// CChips_n

CChips_n::CChips_n(CChips::chip_amount_t c)
  :
  rep_n(c)
{}

CChips CChips_n::ntohl() const
{
  CChips::chip_amount_t rep = ::ntohl(rep_n);
  CChips chips;
  chips.setRep(rep);
  return chips;
}

