/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CWaitPredicate
  Created  : 08/02/2000

  OVERVIEW : This class checks the wait condition.

 ****************************************************************************/

#include "tournament/waitpredicate.h"

CWaitPredicate::~CWaitPredicate()
{}


bool CWaitPredicate::done(long)
{
    return true;
}

void CWaitPredicate::playerReseat(CTable*,
                                  const char* username)
{}
