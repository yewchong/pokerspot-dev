#ifndef __base_stateante_h__
#define __base_stateante_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class StateAnte
  Created  : 06/15/2000

  OVERVIEW : Implements ante state that does the
             chip animation.

 ****************************************************************************/

#include "base/gamestate.h"
#include "base/chipsanimate.h"
#include "network/compat.h"
#include "common/chips.h"

class CTableView;


BEGIN_NAMESPACE(Base);

class StateAnte : public Base::AnimateState
{
public:
public:
  StateAnte(int player, const CChips& chips);
  ~StateAnte();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  int player_;
  CChips chips_;
  Base::ChipsAnimate chipsAnim_;
};


END_NAMESPACE(Base);

#endif