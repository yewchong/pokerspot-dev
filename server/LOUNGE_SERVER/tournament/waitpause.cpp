/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitPause
  Created  : 08/02/2000

  OVERVIEW : This subclass of CWaitPredicate waits until all tables
             have paused.

 ****************************************************************************/

#include "tournament/waitpause.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "lounge/table.h"
#include <time.h>
#include <stdio.h>

CWaitPause::CWaitPause(CTournament* t,
                       long maxWait)
  :
  maxWait_(maxWait),
  tournament_(t)
{
  startTime_ = time(NULL);
}


// Return true if
// - have been waiting longer than 'maxWait' time, or
// - all tables have paused
//
bool CWaitPause::done(long now)
{
    if (now - startTime_ > maxWait_)
    {
        printf("CWaitPause - time exceeded\n");
        return true;
    }

    CBaseTournamentQueue* tq =
      tournament_->getTournamentQueue();

    printf("Not paused: ");

    TableList& tables = tq->getTableList();

    for (TableList::const_iterator tb = tables.begin(),
                                   te = tables.end();
         tb != te;
         ++tb)
    {
        CTable* table = (*tb).get();
        if (!table->isPaused())
        {
            printf("%s", table->getTitle().c_str());
            return false;
        }
    }

    printf("CWaitPause - all tables paused\n");
    return true;
}
