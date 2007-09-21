#ifndef __lounge_playeriterator_h__
#define __lounge_playeriterator_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PlayerIterator
  Created  : 04/16/2000

  OVERVIEW : This class allows the iteration over players
             from outside the Lounge class.

 ****************************************************************************/

#include "psi_win32.h"
#include "ptr.h"
#include "player.h"
#include "loginplayer.h"
#include <hash_map>


typedef list<CLoginPlayer> LoginPlayers;
typedef LoginPlayers::iterator LoginIT;

struct PlayerIterator
{
    // iterate over all players
    PlayerIterator(const LoginPlayers::iterator& lb,
                   const LoginPlayers::iterator& le,
                   const Players::iterator& begin,
                   const Players::iterator& end)
        :
        which_(true),
        lcurr_(lb),
        lend_(le),
        curr_(begin),
        end_(end)
    {}

    // iterate over logged in players
    PlayerIterator(const Players::iterator& begin,
                   const Players::iterator& end)
        :
        which_(false),
        curr_(begin),
        end_(end)
    {}

    PlayerIterator(const PlayerIterator& rhs)
        :
        which_(true),
        lcurr_(rhs.lcurr_),
        lend_(rhs.lend_),
        curr_(rhs.curr_),
        end_(rhs.end_)
    {}

    bool more() const
    {

        if (which_)
        {
            if (lcurr_ != lend_)
                return true;
            else
                const_cast<PlayerIterator*>(this)->which_ = false;
        }
        return curr_ != end_;
    }

    void next();
    CPlayer* current() const;

    bool which_;
    LoginPlayers::iterator lcurr_, lend_;
    Players::iterator curr_, end_;
};

inline void PlayerIterator::next()
{
    if (which_)
    {   // login list
        ++lcurr_;
    }
    else
    {   // players list
        ++curr_;
    }
}

inline CPlayer* PlayerIterator::current() const
{
    if (which_)
        return &(*lcurr_).player_;
    else
        return &(*curr_).second;
}

/*
struct PlayerIteratorC
{
    PlayerIteratorC(const Players::const_iterator& begin,
                    const Players::const_iterator& end)
        : curr_(begin), end_(end) {}

    PlayerIteratorC(const PlayerIteratorC& rhs)
        : curr_(rhs.curr_), end_(rhs.end_) {}

    bool more() const
    {
        return curr_ != end_;
    }

    void next()
    {
        ++curr_;
    }

    CPlayer* current() const
    {
        return (*curr_).second.player_;
    }

    Players::const_iterator curr_, end_;
};
*/

#endif