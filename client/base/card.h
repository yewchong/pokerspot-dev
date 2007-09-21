#ifndef __base_card_h__
#define __base_card_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::Card
  Created  : 25.10.1999

  OVERVIEW : Class for representing a card. Card numbers go from 2-14.

 ****************************************************************************/

#include "base/suit.h"
#include "network/ntwktype.h"

BEGIN_NAMESPACE(Base)


class Card
{
public:

  Card()
    : suit_(SuitBegin), num_(0) // Invalid
  {}

  Card(Suit s, int num)
    : suit_(s), num_(num)
  {}

  Card(u_int16_t s, int num)
      : suit_((Base::Suit)s), num_(num)
  {
    ASSERT(s >= SuitBegin && s < SuitEnd);
  }

  Card(const Card& c)
    : suit_(c.suit_), num_(c.num_)
  {}

  Card& operator=(const Card& rhs)
  {
    if (this != &rhs)
    {
      suit_ = rhs.suit_;
      num_ = rhs.num_;
    }
    return *this;
  }

  Suit suit() const { return suit_; }
  int  number() const { return num_; }
  void suit(Suit s) { suit_ = s; }
  void number(int n) { num_ = n; }

private:
  Suit suit_;
  int  num_;

  friend bool operator == (const Card& lhs, const Card& rhs);
  friend bool operator < (const Card& lhs, const Card& rhs);
};


inline bool operator == (const Card& lhs, const Card& rhs)
{
  return lhs.suit_ == rhs.suit_ &&
         lhs.num_ == rhs.num_;
}

// Sorting cards is based on number, suit is ignored.
inline bool operator < (const Card& lhs, const Card& rhs)
{
  return lhs.num_ < rhs.num_;
}

END_NAMESPACE(Base)

#endif

    