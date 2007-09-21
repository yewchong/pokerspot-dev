/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CQueueService
  Created  : 04/16/2000

  OVERVIEW : This class services those queues that
             require servicing.  By keeping track of
             which queues need to be serviced we avoid
             having to go through all queues in every
             server iteration.

 ****************************************************************************/

#include "lounge/queueservice.h"
#include "lounge/waitinglist.h"
#include "tournament/tournamentqueue.h"
#include "tournament/tournament.h"
#include "dbhandler/dbinterface.h"

enum
{
    TournamentCheckTime = 10, // seconds
    MaxTournamentPlayers = 100
};


static CQueueService g_QueueService;
CQueueService* CQueueService::pInst_ = NULL;

CQueueService::CQueueService()   
{
    pInst_ = this;
}

CQueueService::~CQueueService()
{
    pInst_ = NULL;
}

bool CQueueService::schedule(CWaitingList* wlist)
{
    return (scheduled_.insert(Scheduled::value_type(wlist, wlist)).second);
}


// Go through scheduled waiting lists and
// tick each.
void CQueueService::serviceQueues(long now)
{
    // First remove those waiting lists that
    // requested not to be scheduled any more
    list<CWaitingList*>::iterator it = stopped_.begin(),
                                  end = stopped_.end();
    while (it != end)
    {
        Scheduled::iterator p = scheduled_.find(*it);
        if (p != scheduled_.end())
            scheduled_.erase(p);
        it = stopped_.erase(it);
    }

    // Then go through those waiting lists that are
    // to be scheduled
    for (Scheduled::iterator i = scheduled_.begin(),
                             e = scheduled_.end();
         i != e;
         ++i)
    {
        CWaitingList* wlist = (*i).second;
        wlist->tick(now);
    }

/*  A tournament queue will always be scheduled because it
    never removes itself when players are eliminated
    // Make sure the tournament queue is always scheduled
    if (scheduled_.size() == 0)
    {
        if (CTournamentQueue::GetQueue())
            Schedule(CTournamentQueue::GetQueue());
    }
*/

    static long prev = now - (TournamentCheckTime + 1);
    if (now - prev > TournamentCheckTime)
    {
        prev = now;
        checkTournament();
    }
}


void CQueueService::stopSchedule(CWaitingList* wlist)
{   // will be removed in next iteration
    stopped_.push_back(wlist);
}

// Get tournament parameters from database, update tournament
// if number changed.
void CQueueService::checkTournament()
{
  int queueIndex = 1;
  CTournamentQueue* tq = 
    CTournamentQueue::GetMultiTableTournamentQueue(queueIndex);

  while (tq != NULL)
  {
    bool isRunning = tq->getTournament() && 
                     tq->getTournament()->isRunning();

    if (!isRunning)
    {
      int number = -1, maxPlayers = 10;
      CChips buyin, startChips, startLo;
      u_int32_t startTime = 0;
      string descr, script;

      if (CdbInterface::Inst()->getTournament(tq->getQueueNumber(),
                                              number,
                                              buyin,
                                              startChips,
                                              startLo,
                                              startTime,
                                              maxPlayers,
                                              descr,
                                              script))
      {
        int currentNumber = 0;
        if (tq->getTournament())
          currentNumber = tq->getTournament()->getNumber();

        if (tq->getTournament() && currentNumber != number)
        {
          tq->removeTournament();
        }

        if (number != 0 && number != currentNumber)
        {
          tq->addTournament(number,
                            maxPlayers,
                            buyin,
                            startChips,
                            startLo,
                            startTime,
                            descr,
                            script);
        }
      }
    }
    tq = CTournamentQueue::GetMultiTableTournamentQueue(++queueIndex);
  }
}