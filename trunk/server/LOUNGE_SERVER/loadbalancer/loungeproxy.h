#ifndef __lounge_loungeproxy_h__
#define __lounge_loungeproxy_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoungeProxy
  Created  : 28/09/2001

  OVERVIEW : This class represents one connected Lounge Server.

 ****************************************************************************/

#include "compat.h"
#include <list>
#include <string>
USING_NS_T(std, string);
USING_NS_T(std, list);


class CLoungeProxy
{
public:
  CLoungeProxy(SOCKET sd, u_long addr);
  CLoungeProxy(const CLoungeProxy& rhs);
  CLoungeProxy& operator = (const CLoungeProxy& rhs);
  
  SOCKET  getSocket() const;
  u_long  getAddr() const;
  u_short getPort() const;
  void    setPort(u_short);

  int getNumber() const;
  void setNumber(int);
  bool isLoggedIn() const;

  int  getNetworkError() const;
  void setNetworkError(bool);
    
private:
  SOCKET sd_;

  int number_;
  int networkError_;
  
  u_long addr_;
  u_short port_;
    
  friend bool operator == (const CLoungeProxy& lhs, const CLoungeProxy& rhs);
};

inline SOCKET CLoungeProxy::getSocket() const
{
  return sd_;
}

inline int CLoungeProxy::getNumber() const
{
  return number_;
}

inline u_long CLoungeProxy::getAddr() const
{
  return addr_;
}

inline u_short CLoungeProxy::getPort() const
{
  return port_;
}

inline void CLoungeProxy::setNumber(int number)
{
  number_ = number;
}

inline bool operator == (const CLoungeProxy& lhs, const CLoungeProxy& rhs)
{
  return lhs.number_ == rhs.number_;
}

inline int CLoungeProxy::getNetworkError() const
{
  return networkError_;
}

inline void CLoungeProxy::setNetworkError(bool b)
{
  if (b)
    networkError_++;
  else
    networkError_ = 0;
}

inline void CLoungeProxy::setPort(u_short port)
{
  port_ = port;
}

inline bool CLoungeProxy::isLoggedIn() const
{
  // port is 0 until successfull login
  return port_ > 0 &&
         sd_ != INVALID_SOCKET;
}


#endif
