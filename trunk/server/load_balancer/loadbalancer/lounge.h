#ifndef __lb_lounge_h__
#define __lb_lounge_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoadBalancer
  Created  : 22/09/2001

  OVERVIEW : This class represents one connected Lounge Server.

 ****************************************************************************/

#include "common/compat.h"
#include <list>
#include <string>
USING_NS_T(std, string);
USING_NS_T(std, list);


class CLounge
{
public:
  CLounge(SOCKET sd, u_long addr, int number);
  CLounge(const CLounge& rhs);
  CLounge& operator = (const CLounge& rhs);
  
  SOCKET  getSocket() const;
  u_long  getIpAddress() const;
  u_short getPort() const;
  void    setPort(u_short);

  string getTitle() const;
  int getNumber() const;
  int getNumPlayers() const;
  int getNumTables() const;
  void setNumber(int);
  void setNumPlayers(int);
  void setNumTables(int);
  bool isLoggedIn() const;

  int  getNetworkError() const;
  void setNetworkError(bool);
    
private:
  SOCKET sd_;

  int number_;
  int numPlayers_;
  int numTables_;
  int networkError_;
  
  u_long addr_;
  u_short port_;
    
  friend bool operator == (const CLounge& lhs, const CLounge& rhs);
};

inline SOCKET CLounge::getSocket() const
{
  return sd_;
}

inline int CLounge::getNumber() const
{
  return number_;
}

inline int CLounge::getNumPlayers() const
{
  return numPlayers_;
}

inline int CLounge::getNumTables() const
{
  return numTables_;
}

inline u_long CLounge::getIpAddress() const
{
  return addr_;
}

inline u_short CLounge::getPort() const
{
  return port_;
}

inline void CLounge::setNumPlayers(int numPlayers)
{
  numPlayers_ = numPlayers;
}

inline void CLounge::setNumTables(int numTables)
{
  numTables_ = numTables;
}

inline void CLounge::setNumber(int number)
{
  number_ = number;
}

inline bool operator == (const CLounge& lhs, const CLounge& rhs)
{
  return lhs.number_ == rhs.number_;
}

inline int CLounge::getNetworkError() const
{
  return networkError_;
}

inline void CLounge::setNetworkError(bool b)
{
  if (b)
    networkError_++;
  else
    networkError_ = 0;
}

inline void CLounge::setPort(u_short port)
{
  port_ = port;
}

inline bool CLounge::isLoggedIn() const
{
  // port is 0 until successfull login
  return port_ > 0 &&
         sd_ != INVALID_SOCKET;
}


#endif
