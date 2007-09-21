#ifndef __base_statedealcommunity2_h__
#define __base_statedealcommunity2_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class StateDealCommunity2
  Created  : 04/11/2000

  OVERVIEW : Deal Community cards state. Can deal any number
             of cards.

 ****************************************************************************/


#include "base/gamestate.h"
#include "base/card.h"
#include "network/pdudealcardsflex.h"
#include <list>


class CTableView;

class StateDealCommunity2 : public Base::AnimateState
{
public:
  typedef std::list<Base::Card> DealCards;

public:
  StateDealCommunity2(int numCards,
                      Network::CardEntry*,
                      int msecs);
  ~StateDealCommunity2();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  int currentNum_;
  int dealerNum_;
  int msecs_;
  CTableView* pTableView_;

  DealCards dealCards_;
  int numCards_;
};


#endif