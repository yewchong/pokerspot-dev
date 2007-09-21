/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateShowdown
  Created  : 04/27/2000

  OVERVIEW : Implements showdown state:
                - does muck cards animations 

 ****************************************************************************/

#include "stdafx.h"
#include "base/stateshowdown.h"
#include "tem/playerindex.h"
#include "base/player.h"
#include "base/cards.h"
#include "ui/tableview.h"
#include "ui/global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using Base::Cards;
using Base::Card;

//
// STATE: StateShowdown
//        Do muck cards animation for a player at showdown.
//
// Entry: Showdown PDU has arrived, player mucks cards
//
// Exit:  Next state.
//

StateShowdown::StateShowdown(int player, int numCards)
  :
  Base::AnimateState(CTableView::Instance()),
  player_(player),
  numCards_(numCards)
{}


StateShowdown::~StateShowdown()
{
  if (cardsAnim_.isAnimationOn())
    cardsAnim_.stopAnimate();
}


BOOL StateShowdown::tick(DWORD dt)
{
  if (player_ != -1)
  {
    cardsAnim_.startAnimate(player_, PI_House,
                            numCards_,
                            Global::GetAnimationTime());
    player_ = -1;
    dt = 1;
  }

  return cardsAnim_.stepAnimate(dt);
}


BOOL StateShowdown::draw(CDC* pDC)
{
  return FALSE;
}


BOOL StateShowdown::onCommand(WPARAM wParam, LPARAM lParam)
{
  return FALSE;
}



