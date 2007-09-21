/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class StateAnte
  Created  : 06/15/2000

  OVERVIEW : Implements ante state that does the
             chip animation.

 ****************************************************************************/

#include "stdafx.h"
#include "base/stateante.h"
#include "tem/playerindex.h"
#include "base/player.h"
#include "base/graphic_chips.h"
#include "ui/tableview.h"
#include "ui/global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_NAMESPACE(Base);

using Base::Chips;

//
// STATE: StateAnte
//        Do chip animation for a player.
//
// Entry: Action echo PDU arrived, player antes.
//
// Exit:  Next state.
//

StateAnte::StateAnte(int player, const CChips& chips)
  :
  Base::AnimateState(CTableView::Instance()),
  player_(player),
  chips_ (chips)
{}


StateAnte::~StateAnte()
{
  chipsAnim_.stopAnimate();
}


BOOL StateAnte::tick(DWORD dt)
{
  if (player_ != -1)
  {
    chipsAnim_.startAnimate(player_,
                            PI_Community,
                            chips_);
    player_ = -1;
    dt = 1;
  }

  return chipsAnim_.stepAnimate(dt);
}


BOOL StateAnte::draw(CDC* pDC)
{
  return FALSE;
}


BOOL StateAnte::onCommand(WPARAM wParam, LPARAM lParam)
{
  return FALSE;
}


END_NAMESPACE(Base);