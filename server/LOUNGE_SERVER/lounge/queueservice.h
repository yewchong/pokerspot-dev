#ifndef __lounge_queueservice_h__
#define __lounge_queueservice_h__

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

#include "def.h"
#include <list>
#include <map>
#include <algorithm>
#include "compat.h"
USING_NS_T(std, list);
USING_NS_T(std, map);
USING_NS_T(std, less);

class CWaitingList;


class CQueueService
{
public:
    static bool Schedule(CWaitingList* wlist);
    static void ServiceQueues(long now);
    static void StopSchedule(CWaitingList* wlist);

public:
    bool schedule(CWaitingList* wlist);
    void serviceQueues(long now);
    void stopSchedule(CWaitingList* wlist);

    void checkTournament();

public:
    CQueueService();
    ~CQueueService();

private:
    list<CWaitingList*> stopped_;
    typedef map<CWaitingList*, CWaitingList*, less<CWaitingList*> > Scheduled;
    Scheduled scheduled_;

    static CQueueService* pInst_;
};


inline bool CQueueService::Schedule(CWaitingList* waitingList)
{
    return pInst_->schedule(waitingList);
}

inline void CQueueService::ServiceQueues(long now)
{
    pInst_->serviceQueues(now);
}

inline void CQueueService::StopSchedule(CWaitingList* wlist)
{
    pInst_->stopSchedule(wlist);
}


#endif
