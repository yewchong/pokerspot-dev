#ifndef __base_statedealpocket2_h__
#define __base_statedealpocket2_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class StateDealPocket2
  Created  : 04/11/2000

  OVERVIEW : New Deal Pocket cards state. Can deal any number
             of pocket cards.

 ****************************************************************************/


#include "base/gamestate.h"
#include "base/card.h"
#include "network/pdudealcardsflex.h"
#include <list>


class CTableView;

class StateDealPocket2 : public Base::AnimateState
{
public:
  typedef std::list<Base::Card> DealCards;

public:
  StateDealPocket2(int dealer,
                   int numCards,
                   Network::CardEntry*,
                   int msecs);
  // This version is for dummy cards
  StateDealPocket2(int dealer,
                   int numCards,
                   int msecs);
  ~StateDealPocket2();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  void finishDealCards();
  int currentNum_, firstPlayer_;
  int dealerNum_;
  int msecs_;
  CTableView* pTableView_;

  DealCards cards_;
  int numCards_;
  bool dummy_;
#ifdef _DEBUG
  int numIters_;
#endif
};


#endif