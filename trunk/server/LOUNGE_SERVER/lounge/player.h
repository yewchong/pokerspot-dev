#ifndef __lounge_player_h__
#define __lounge_player_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : Class CPlayer
  Created  : 04/16/2000

  OVERVIEW : Player class.

 ****************************************************************************/

#include <string>
#include <hash_map>
#include <map>
#include "compat.h"
#include "def.h"
#include "pingcounter.h"
USING_NS_T(std, hash_map);
USING_NS_T(std, map);
USING_NS_T(std, hash);
USING_NS_T(std, string);

class Cpdu;

#ifdef _WIN32
#pragma warning(disable : 4355)
#endif


class CPlayer
{
public:
  CPlayer()
    : sd_(INVALID_SOCKET),
      ipaddress_(0),
      networkErr_(false),
      pingCounter_(this)
  {}

  CPlayer(const string& s)
    : name_(s),
      sd_(INVALID_SOCKET),
      ipaddress_(0),
      networkErr_(false),
      pingCounter_(this)
  {}

  CPlayer(SOCKET sd, u_long ipaddress)
    : sd_(sd),
      ipaddress_(ipaddress),
      networkErr_(false),
      pingCounter_(this)
  {}

  CPlayer(const string& s, SOCKET sd, u_long ipaddress)
    : name_(s),
      sd_(sd),
      ipaddress_(ipaddress),
      networkErr_(false),
      pingCounter_(this)
  {}

  CPlayer(const CPlayer& p)
    : name_    (p.name_),
      password_(p.password_),
      sd_      (p.sd_),
      ipaddress_(p.ipaddress_),
      networkErr_(p.networkErr_),
      pingCounter_(this)
  {}

  CPlayer& operator = (const CPlayer& rhs)
  {
    if (this != &rhs)
    {
      name_ = rhs.name_;
      password_ = rhs.password_;
      sd_ = rhs.sd_;
      ipaddress_ = rhs.ipaddress_;
      networkErr_ = rhs.networkErr_;
    }
    return *this;
  }

  virtual ~CPlayer() {}

  SOCKET getSocket() const
  {
    return sd_;
  }

  void setSocket(SOCKET sd)
  {
    sd_ = sd;
  }

  string name() const
  {
    return name_;
  }

  const char* getUsername() const
  {
    return name_.c_str();
  }

  u_long getIpAddress() const
  {
    return ipaddress_;
  }

  void setNetworkError(bool b)
  {
    networkErr_ = b;
  }

  bool getNetworkError() const
  {
    return networkErr_;
  }

  virtual CPingCounter* getPingCounter();
  virtual int send(Cpdu&);


private:
  string name_;
  string password_;

  SOCKET sd_;        // player's socket 
  u_long ipaddress_;
  bool   networkErr_;

  CPingCounter pingCounter_;
  
  friend bool operator == (const CPlayer&, const CPlayer&);
  friend bool operator < (const CPlayer&, const CPlayer&);

};


inline bool operator == (const CPlayer& lhs, const CPlayer& rhs)
{
  return lhs.name_ == rhs.name_;
}


inline bool operator < (const CPlayer& lhs, const CPlayer& rhs)
{
  return lhs.name_ < rhs.name_;
}

// In some cases the compiler is not able to
// resolve the string hasher - give one explicitely
struct hash_string
{
  size_t operator() (const string& s) const
  {
    hash<const char*> hasher;
    return hasher(s.c_str());
  }
};

typedef hash_map<string, CPlayer, hash_string> Players;

#ifdef _WIN32
#pragma warning(default : 4355)
#endif

#endif
