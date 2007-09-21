#ifndef __base_statedealupcard_h__
#define __base_statedealupcard_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class StateDealUpCard
  Created  : 06/22/2000

  OVERVIEW : Deal up cards to specified player.

 ****************************************************************************/


#include "base/gamestate.h"
#include "base/card.h"
#include "network/pdudealcardsflex.h"
#include <list>


class CTableView;

class StateDealUpCard : public Base::AnimateState
{
public:
  typedef std::list<Base::Card> DealCards;

public:
  StateDealUpCard(int player,                  
                  int numCards,
                  Network::CardEntry*,
                  int msecs);
  ~StateDealUpCard();

  void dealUpCard(int player,
                  int numCards,
                  Network::CardEntry*,
                  int msecs);

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  void finishCurrentDeal();
  int player_;
  int msecs_;
  CTableView* pTableView_;

  DealCards cards_;
};


#endif