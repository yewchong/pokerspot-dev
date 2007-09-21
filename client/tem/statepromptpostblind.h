#ifndef __tem_statepromptpostblind_h__
#define __tem_statepromptpostblind_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StatePromptPostBlind
  Created  : 11.11.1999

  OVERVIEW : Prompt the user whether to post the blind.

 ****************************************************************************/

#include "base/gamestate.h"
#include "base/turnindicator.h"
#include "base/chipsanimate.h"
#include "common/chips.h"

class CTableView;

class StatePromptPostBlind : public Base::GameState
{
public:
  StatePromptPostBlind(int player,
                       const CChips& value,
                       unsigned int msecs);
  ~StatePromptPostBlind();

  void startPrompt(int player,
                   const CChips& value,
                   unsigned int msecs);
  void endPrompt();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);
  BOOL onKey(UINT nChar, UINT nRepCnt, UINT nFlags);
  BOOL onTimer(UINT msg, UINT eventId, DWORD dwTime);

  void start();

private:
  int    player_;
  CChips value_;
  DWORD  ticks_;
  DWORD  waitActionTime_, timeLeft_;
  UINT   timer_;
  
  CTableView* pView_;

  Base::TurnIndicator turnPtr_;

  // Support for chips animation
  Base::ChipsAnimate chipsAnim_;
  int doChipsAnimate_;
};


#endif