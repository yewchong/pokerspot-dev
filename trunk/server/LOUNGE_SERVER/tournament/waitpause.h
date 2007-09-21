#ifndef __lounge_waitpause_h__
#define __lounge_waitpause_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitPause
  Created  : 08/02/2000

  OVERVIEW : This subclass of CWaitPredicate waits until all tables
             have paused.

 ****************************************************************************/

#include "tournament/waitpredicate.h"

class CTournament;

class CWaitPause : public CWaitPredicate 
{
public:
    CWaitPause(CTournament* t,
               long maxWait);

    virtual bool done(long now);

private:
    long startTime_, maxWait_;
    CTournament* tournament_;
};


#endif