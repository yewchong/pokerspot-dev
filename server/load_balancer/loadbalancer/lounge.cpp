/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CLounge
  Created  : 04/16/2000

  OVERVIEW : This struct maintains information about a
             table server.

 ****************************************************************************/

#include "loadbalancer/lounge.h"
#include "common/stringoutput.h"


CLounge::CLounge(SOCKET sd, u_long addr, int number)
  :
  sd_          (sd),
  number_      (number),
  numPlayers_  (0),
  numTables_   (0),
  addr_        (addr),
  port_        (0),
  networkError_(0)
{}

CLounge::CLounge(const CLounge& rhs)
  :
  sd_         (rhs.sd_),
  number_     (rhs.number_),
  numPlayers_ (rhs.numPlayers_),
  numTables_  (rhs.numTables_),
  addr_       (rhs.addr_),
  port_       (rhs.port_),
  networkError_(rhs.networkError_)
{}

CLounge& CLounge::operator = (const CLounge& rhs)
{
  if (this != &rhs)
  {
    sd_ = rhs.sd_;
    number_ = rhs.number_;
    numPlayers_ = rhs.numPlayers_;
    numTables_ = rhs.numTables_;
    addr_ = rhs.addr_;
    port_ = rhs.port_;
    networkError_ = rhs.networkError_;
  }
  return *this;
}

string CLounge::getTitle() const
{
  CStrOut buf;
  buf << "Lounge " << number_;
  return string(buf.str());
}
