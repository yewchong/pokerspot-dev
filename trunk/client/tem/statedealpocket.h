#ifndef __tem_statedealpocket_h__
#define __tem_statedealpocket_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateDealPocket
  Created  : 10.11.1999

  OVERVIEW : Implements the dealing of pocket cards for Texas Hold'em.

 ****************************************************************************/


#include "base/gamestate.h"


class CTableView;

class StateDealPocket : public Base::AnimateState
{
public:
  enum {
    NumFacedownCards    = 2,
    DefaultDealCardTime = 90
  };

public:
  StateDealPocket(int dealer,
                  int card1, int suit1,
                  int card2, int suit2,
                  int msecs);
  ~StateDealPocket();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  int currentNum_;
  int dealerNum_;
  CTableView* pTableView_;

  int msecs_;
  
  int card1_, suit1_,
      card2_, suit2_;
};


#endif