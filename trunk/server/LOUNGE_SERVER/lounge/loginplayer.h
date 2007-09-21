#ifndef __lounge_loginplayer_h__
#define __lounge_loginplayer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct LoginPlayer
  Created  : 04/16/2000

  OVERVIEW : This class is used to hold players who are
             logging in.
             
             It associates a time stamp with a player who
             is in the process of logging in. If the login
             lasts longer than a specified time, the connection
             is terminated.

 ****************************************************************************/

#include "player.h"
#include <time.h>

struct CLoginPlayer
{
    CLoginPlayer()
        : retryCount_(0)
    {
        time(&loginTime_);
    }

    CLoginPlayer(SOCKET sd, u_long ipaddress)
        : player_(sd, ipaddress), retryCount_(0)
    {
        time(&loginTime_);
    }

    CLoginPlayer(const CLoginPlayer& rhs)
        :
        player_    (rhs.player_),
        loginTime_ (rhs.loginTime_),
        retryCount_(rhs.retryCount_),
        username_  (rhs.username_),
        password_  (rhs.password_)
    {}

    CLoginPlayer& operator = (const CLoginPlayer& rhs)
    {
        if (this != &rhs)
        {
            loginTime_ = rhs.loginTime_;
            player_ = rhs.player_;
            retryCount_ = rhs.retryCount_;
            username_ = rhs.username_;
            password_ = rhs.password_;
        }
        return *this;
    }

    CPlayer player_;
    long    loginTime_;
    int     retryCount_;

    string  username_;
    string  password_;
};

inline bool operator == (const CLoginPlayer& lhs, const CLoginPlayer& rhs)
{
    return lhs.player_.getSocket() == rhs.player_.getSocket();
}


#endif