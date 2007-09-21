#ifndef __lb_loungeptr_h__
#define __lb_loungeptr_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class LoungePtr
  Created  : 22/09/2001

  OVERVIEW : Semi-smart pointer class. The CLounge objects
             are maintained in primary containers. Other places have
             containers of LoungePtrs that refer to objects
             in the primary containers. This avoids duplication of CLounge
             objects.

 ****************************************************************************/

#include "loadbalancer/lounge.h"
#include <functional>


struct LoungePtr
{
  LoungePtr()
    : lounge_(0) {}
  
  LoungePtr(const LoungePtr& rhs)
    : lounge_(rhs.lounge_) {}
  
  LoungePtr(CLounge& lounge)
    : lounge_(&lounge) {}
  
  LoungePtr& operator = (const LoungePtr& rhs)
  {
    if (this != &rhs)
    {
      lounge_ = rhs.lounge_;
    }
    return *this;
  }
  
  CLounge* operator->() const
  {
    return lounge_;
  }
  CLounge* get() const
  { 
    return lounge_;
  }
  
  CLounge* lounge_;
  
  friend bool operator == (const LoungePtr& lhs, const LoungePtr& rhs);
};


inline bool operator == (const LoungePtr& lhs, const LoungePtr& rhs)
{
  return *lhs.lounge_ == *rhs.lounge_;
}


//
// Define functors that allow searching of CLounges
// with certain criteria.
//

struct same_socket : public std::unary_function<const LoungePtr&, bool>
{
  same_socket(SOCKET sd)
    : sd_(sd)
  {}
  bool operator()(const CLounge& l) const
  {
    return l.getSocket() == sd_;
  }
  bool operator()(const LoungePtr& ptr) const
  {
    return ptr.lounge_->getSocket() == sd_;
  }
  
  SOCKET sd_;
};


struct same_number : public std::unary_function<const LoungePtr&, bool>
{
    same_number(int number)
        : number_(number)
    {}
    bool operator() (const LoungePtr& ptr) const
    {
        return ptr.lounge_->getNumber() == number_;
    }
    int number_;
};



#endif
