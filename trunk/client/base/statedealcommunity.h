#ifndef __tem_statedealcommunity_h__
#define __tem_statedealcommunity_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class StateDealCommunity
  Created  : 12.11.1999

  OVERVIEW : Implements the dealing of community cards for Texas Hold'em.

 ****************************************************************************/


#include "base/gamestate.h"
#include "base/card.h"
#include "network/pdudealcardsflex.h"
#include <list>


class CTableView;

class StateDealCommunity : public Base::AnimateState
{
public:
  StateDealCommunity(int cards,
                     int card1, int suit1,
                     int card2, int suit2,
                     int card3, int suit3,
                     int msecs);
  StateDealCommunity(int numCards,
                     Network::CardEntry*,                     
                     int msecs);
  ~StateDealCommunity();

  BOOL tick(DWORD dt);
  BOOL draw(CDC*);
  BOOL onCommand(WPARAM, LPARAM);

private:
  CTableView* pView_;

  int msecs_;
  DWORD subtractTime_;
  
  int numCards_;
  std::list<Base::Card> cards_;
};


#endif