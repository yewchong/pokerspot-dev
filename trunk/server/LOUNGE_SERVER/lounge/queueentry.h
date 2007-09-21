#ifndef __lounge_queueentry_h__
#define __lounge_queueentry_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct QueueEntry
  Created  : 04/27/2000

  OVERVIEW : This struct maintains information about a player in a
             queue (aka waiting list).

 ****************************************************************************/

#include "compat.h"
#include "ptr.h"

enum QueueEntry_State
{
    QE_Waiting     = 0,
    QE_BeingSeated = 1
};

// Player in a queue is represented by
// a 'QueueEntry'. 
struct QueueEntry
{
    QueueEntry()
        : state_(QE_Waiting) {}
    QueueEntry(const QueueEntry& qe)
        : player_(qe.player_), state_(qe.state_) {}
    QueueEntry& operator = (const QueueEntry& rhs)
    {
        if (this != &rhs)
        {
            player_ = rhs.player_;
            state_ = rhs.state_;
        }
        return *this;
    }
    QueueEntry(CPlayer& player)
        : player_(player), state_(QE_Waiting) {}

    PlayerPtr player_;
    QueueEntry_State state_;
};

// QueueEntry and PlayerPtr are equal if
// they refer to the same player

inline bool operator == (const QueueEntry& lhs, const PlayerPtr& rhs)
{
    return lhs.player_ == rhs;
}

inline bool operator == (const PlayerPtr& lhs, const QueueEntry& rhs)
{
    return lhs == rhs.player_;
}

inline bool operator != (const QueueEntry& lhs, const PlayerPtr& rhs)
{
    return !(lhs.player_ == rhs);
}

inline bool operator != (const PlayerPtr& lhs, const QueueEntry& rhs)
{
    return !(lhs == rhs.player_);
}


#endif