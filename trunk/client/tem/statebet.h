#ifndef __tem_statebet_h__
#define __tem_statebet_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateBet
  Created  : 11.11.1999

  OVERVIEW : Implements one betting round for Texas Hold'em.

 ****************************************************************************/


#include "base/gamestate.h"
#include "base/turnindicator.h"
#include "base/chipsanimate.h"
#include "base/cardsanimate.h"
#include "common/chips.h"

namespace Base
{
  class Player;
}

class CTableView;
class CActionBar;

//
// STATE: First betting round.
//
class StateBet : public Base::GameState
{
public:
  StateBet();
  ~StateBet();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);
  BOOL onKey(UINT nChar, UINT nRepCnt, UINT nFlags);
  BOOL onTimer(UINT msg, UINT eventId, DWORD dwTime);

protected:
  BOOL onCall();
  BOOL onRaise();
  BOOL onFold();
  BOOL onActionRequest(Base::Player* pPot, LPARAM lParam);
  BOOL onActionRequestSpreadLimit(Base::Player* pPot, LPARAM lParam);
  BOOL onAction(Base::Player* pPot, LPARAM lParam);

  void  startPrompt(int time, int player, const CChips& value, const CChips& raise);
  void  startPrompt(int time, int player, const CChips&value, const CChips&raiseLo, const CChips& raiseHi);
  void  endPrompt();

private:
  DWORD        ticks_;
  DWORD        waitActionTime_;
  int          currentPlayer_;
  CChips       bet_, raise_;
  bool         prompting_;
  UINT         timer_;
  CChips       raiseLo_, raiseHi_;
  bool         spreadLimit_;

  CTableView* pView_;

  // The "your turn" pointer
  Base::TurnIndicator turnPtr_;

  // Support for chips animation
  Base::ChipsAnimate chipsAnim_;
  int doChipsAnimate_;
  CChips chipsAmount_;

  // Support for fold animation
  Base::CardsAnimate cardsAnim_;
  int doFoldAnimate_;
  int numFoldCards_;
};


#endif