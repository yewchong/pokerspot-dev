/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : initgame.cpp
  Created  : 25.10.1999

  OVERVIEW : Implements the Texas Hold'em entry point functions.

 ****************************************************************************/

#include "stdafx.h"
#include "tem/remoteplayer.h"
#include "tem/house.h"
#include "tem/community.h"
#include "tem/playerindex.h"
#include "base/gamestate.h"
#include "ui/tableview.h"

namespace
{
  const CString g_PlayerTitle("Player #%i");
}


Base::GameState* CreateStartState();
void InitGame(CTableView*);
void RecalcLayout(CTableView*);


//
// Upon joining a table, this function is
// called
//
Base::GameState* CreateStartState()
{
  // No special start state
  return 0;
}


void InitGame(CTableView* pView)
{ 
  // Create the house and the pot players
}


// 10 players + house + community
void RecalcLayout(CTableView* pView)
{
}




