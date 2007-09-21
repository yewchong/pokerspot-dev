#ifndef __lb_player2lounge_h__
#define __lb_player2lounge_h__


/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CLoadBalancer
  Created  : 22/09/2001

  OVERVIEW : This class manages the connected Lounge Servers
             and does the load balancing logic.

 ****************************************************************************/


#include <hash_map>
#include <map>
#include <string>


// In some cases the compiler is not able to
// resolve the string hasher - give one explicitely
struct hash_string : 
  public std::unary_function<const std::string&, size_t>
{
  size_t operator() (const std::string& s) const
  {
    std::hash<const char*> hasher;
    return hasher(s.c_str());
  }
};


typedef std::hash_map<string, CLounge*, hash_string> Player2Lounge;
//typedef std::map<string, CLounge*, hash_string> Player2Lounge;


struct same_lounge : 
  public std::unary_function<const Player2Lounge::value_type&, bool>
{
  same_lounge(CLounge* lounge)
    : lounge_(lounge) {}

  bool operator() (const Player2Lounge::value_type& val) const
  {
    return val.second == lounge_;
  }

  CLounge* lounge_;
};


struct print_contents : 
  public std::unary_function<const Player2Lounge::value_type&, void>
{
  void operator() (const Player2Lounge::value_type& val) const
  {
    cout << "Lounge " << val.second->getNumber() << ' ' 
         << " player " << val.first << endl;
  }

  CLounge* lounge_;
};



#endif