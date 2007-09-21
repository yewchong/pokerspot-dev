#ifndef __base_statepromptstartplay_h__
#define __base_statepromptstartplay_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StatePromptStartPlay
  Created  : 28.12.1999

  OVERVIEW : Prompts the user to start playing upon having logged
             in to a table.

 ****************************************************************************/


#include "base/gamestate.h"
#include "network/network.h"


class StatePromptStartPlay : public Base::GameState
{
public:
  StatePromptStartPlay();
  ~StatePromptStartPlay();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);

  BOOL onCommand(WPARAM, LPARAM);
  BOOL onKey(UINT nChar, UINT nRepCnt, UINT nFlags);
  BOOL onTimer(UINT msg, UINT eventId, DWORD dwTime);
};


#endif
