#ifndef __lounge_waitpredicate_h__
#define __lounge_waitpredicate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitPredicate
  Created  : 08/02/2000

  OVERVIEW : This class checks the wait condition.

 ****************************************************************************/

class CTable;

class CWaitPredicate 
{
public:
    virtual ~CWaitPredicate();
    virtual bool done(long now);
    virtual void playerReseat(CTable*,
                              const char* username);
};


#endif