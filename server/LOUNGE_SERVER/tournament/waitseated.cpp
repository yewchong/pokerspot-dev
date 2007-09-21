/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitSeated
  Created  : 08/06/2000

  OVERVIEW : This subclass of CWaitPredicate waits until players
             have been reseated in a dissolve.

 ****************************************************************************/

#include "tournament/waitseated.h"
#include <time.h>
#include <stdio.h>
#include <algorithm>

#ifdef _WIN32
using namespace std;
#endif

CWaitSeated::CWaitSeated(const list<string>& players, 
                         long                maxWait)
    :
    maxWait_(maxWait),
    minWait_(0),
    players_(players)
{
    startTime_ = time(NULL);
}


// Return true if
// - have been waiting longer than 'maxWait' time, or
// - all players have been reseated
//
bool CWaitSeated::done(long now)
{
    if (players_.size() == 0)
    {
        // Wait 10 seconds more after seating is complete
        if (minWait_ == 0)
            minWait_ = (now - startTime_) + 12;

        if (now - startTime_ > minWait_)
        {
            printf("CWaitSeated - all players seated\n");
            return true;
        }
    }

    if (now - startTime_ > maxWait_)
    {
        printf("CWaitSeated - time exceeded\n");
        return true;
    }

    return false;
}

void CWaitSeated::playerReseat(CTable*,
                               const char* username)
{
    printf("CWaitSeated: playerReseat %s", username);

    string name(username);

    list<string>::iterator pos =
        find(players_.begin(), players_.end(), name);

    if (pos != players_.end())
        players_.erase(pos);
}
