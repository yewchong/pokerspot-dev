#ifndef __lounge_waitseated_h__
#define __lounge_waitseated_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitSeated
  Created  : 08/06/2000

  OVERVIEW : This subclass of CWaitPredicate waits until players
             have been reseated in a dissolve.

 ****************************************************************************/

#include "tournament/waitpredicate.h"
#include <list>
#include <string>
#include "compat.h"
USING_NS_T(std, string);
USING_NS_T(std, list);


class CWaitSeated : public CWaitPredicate 
{
public:
    CWaitSeated(const list<string>& players,
                long                maxWait);

    virtual bool done(long now);
    virtual void playerReseat(CTable*,
                              const char* username);

private:
    long startTime_, maxWait_, minWait_;
    list<string> players_;
};


#endif