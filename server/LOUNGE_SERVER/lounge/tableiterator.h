#ifndef __lounge_tableiterator_h__
#define __lounge_tableiterator_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct TableIterator
  Created  : 04/16/2000

  OVERVIEW : This class allows the iteration over tables
             from outside the Lounge class.

 ****************************************************************************/

#include "ptr.h"
#include <list>
USING_NS_T(std, map);


typedef list<CTable> Tables;


struct TableIterator
{
    TableIterator(const Tables::iterator& begin,
                  const Tables::iterator& end)
        : curr_(begin), end_(end) {}

    TableIterator(const TableIterator& rhs)
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

    CTable* current() const
    {
        return &(*curr_);
    }

    Tables::iterator curr_, end_;
};

struct TableIteratorC
{
    TableIteratorC(const Tables::const_iterator& begin,
                   const Tables::const_iterator& end)
        : curr_(begin), end_(end) {}

    TableIteratorC(const TableIterator& rhs)
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

    const CTable* current() const
    {
        return &(*curr_);
    }

    Tables::const_iterator curr_, end_;
};


#endif