#ifndef __lb_loungeiterator_h__
#define __lb_loungeiterator_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : struct LoungeIterator
  Created  : 22/09/2001

  OVERVIEW : This class allows the iteration over lounges
             from outside the CLoadBalancer class.

 ****************************************************************************/

#include <list>
USING_NS_T(std, list);

class CLounge;
typedef list<CLounge> Lounges;


struct LoungeIterator
{
  LoungeIterator(const Lounges::iterator& begin,
    const Lounges::iterator& end)
    : curr_(begin), end_(end) {}
  
  LoungeIterator(const LoungeIterator& rhs)
    : curr_(rhs.curr_), end_(rhs.end_)
  {}
  
  bool more() const
  {
    return curr_ != end_;
  }
  
  void next()
  {
    ++curr_;
  }
  
  CLounge* current() const
  {
    return &(*curr_);
  }
  
  Lounges::iterator curr_, end_;
};

struct LoungeIteratorC
{
  LoungeIteratorC(const Lounges::const_iterator& begin,
    const Lounges::const_iterator& end)
    : curr_(begin), end_(end) {}
  
  LoungeIteratorC(const LoungeIterator& rhs)
    : curr_(rhs.curr_), end_(rhs.end_)
  {}
  
  bool more() const
  {
    return curr_ != end_;
  }
  
  void next()
  {
    ++curr_;
  }
  
  const CLounge* current() const
  {
    return &(*curr_);
  }
  
  Lounges::const_iterator curr_, end_;
};


#endif