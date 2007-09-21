#ifndef __table_winner_h__
#define __table_winner_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CWinner
  Created  : 1/1/2001

  OVERVIEW : This structure is used to compute winning
             player's chips.

             If there are more than one player eligible to a pot,
             the chips left over after the division
             (chips in pot / number of players) are distributed evenly
             between the players, beginning from the player closest to
             left of dealer.

 ****************************************************************************/

#include "common/chips.h"

struct CWinner
{
    CWinner()
        : player_(NULL), chips_(0) {}
    CWinner(CPlayer* p)
        : player_(p), chips_(0) {}
    CWinner(const CWinner& w)
        : player_(w.player_), chips_(w.chips_) {}
    CWinner& operator = (const CWinner& rhs)
    {
        if (this != &rhs)
        {
            player_ = rhs.player_;
            chips_ = rhs.chips_;
        }
        return *this;
    }
    CPlayer* player_;
    CChips chips_;
};

#include <set>
typedef set<CWinner> Winners;

// Order based on slot
inline bool operator < (const CWinner& lhs, const CWinner& rhs)
{
    return lhs.player_->getSlot() < rhs.player_->getSlot();
}

// Equality based on slot
inline bool operator == (const CWinner& lhs, const CWinner& rhs)
{
    return lhs.player_->getSlot() == rhs.player_->getSlot();
}


#endif