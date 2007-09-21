#ifndef __base_stateshowdown_h__
#define __base_stateshowdown_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateShowdown
  Created  : 04/27/2000

  OVERVIEW : Implements showdown state:
                - does muck cards animation for one player

 ****************************************************************************/

#include "base/gamestate.h"
#include "base/cardsanimate.h"

class CTableView;

class StateShowdown : public Base::AnimateState
{
public:
public:
  StateShowdown(int player, int numCards);
  ~StateShowdown();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  int player_, numCards_;
  Base::CardsAnimate cardsAnim_;
};


#endif