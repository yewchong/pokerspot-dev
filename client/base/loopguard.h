#ifndef __base_loopguard_h__
#define __base_loppguard_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class LoopGuard
  Created  : 01/16/2000

  OVERVIEW : Helper class for looping over players.

 ****************************************************************************/


// This class is used for preventing infinite
// loops when iterating over players.
//
// Extra care is taken in detecting loop end
// because players may vanish from the table
// at any moment.

struct LoopGuard
{
  LoopGuard()
  {
    for (int i = 0; i < 10; i++)
      visit_[i] = false;
  }

  bool visit(int index)
  {
    bool rc = false;

    if (index >= 0 && index < 10)
    {
      rc = !(visit_[index]);
      visit_[index] = true;
    }

    return rc;
  }

  bool visited(int index) const
  {
    return visit_[index];
  }

  bool visit_[10];
};


#endif