#ifndef __lounge_playerptr_h__
#define __lounge_playerptr_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PlayerPtr
  Created  : 04/16/2000

  OVERVIEW : Semi-smart pointer class. The CPlayer and CTable objects
             are maintained in primary containers. Other places have
             containers of PlayerPtrs and TablePtrs that refer to objects
             in the primary containers. This avoids duplication of CPlayer
             and CTable objects.

 ****************************************************************************/

#include "lounge/player.h"
#include "lounge/loginplayer.h"


struct PlayerPtr
{
    PlayerPtr()
        : player_(0) {}

    PlayerPtr(const PlayerPtr& rhs)
        : player_(rhs.player_) {}

    PlayerPtr(CPlayer& player)
        : player_(&player) {}

    PlayerPtr& operator = (const PlayerPtr& rhs)
    {
        if (this != &rhs)
        {
            player_ = rhs.player_;
        }
        return *this;
    }

    CPlayer* operator->() const
    {
        return player_;
    }
    CPlayer* get() const
    { 
        return player_;
    }

    CPlayer* player_;

    friend bool operator == (const PlayerPtr& lhs, const PlayerPtr& rhs);
};

inline bool operator == (const PlayerPtr& lhs, const PlayerPtr& rhs)
{
    return *lhs.player_ == *rhs.player_;
}

struct same_name
{
    same_name(const string& name)
        : name_(name)
    {}
    bool operator()(const PlayerPtr& ptr) const
    {
        return strcmp(ptr.player_->getUsername(),
                      name_.c_str()) == 0;
    }

    string name_;
};

#endif