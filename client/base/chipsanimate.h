#ifndef __base_chipanimate_h__
#define __base_chipanimate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Base::ChipAnimate
  Created  : 03/03/2000

  OVERVIEW : A class for doing chips animation.

 ****************************************************************************/


#include "common/chips.h"


class CTableView;


namespace Base
{

class ChipsAnimate
{
public:
  ChipsAnimate();

  void startAnimate(int from, int to,
                    const CChips& chips,
                    int sound = -1);
  void stopAnimate();
  BOOL stepAnimate(DWORD dt);

private:
  void animate(const CPoint& newPos,
               const CPoint& oldPos);

  CChips chips_;
  DWORD  ticks_, msecs_;
  CPoint start_, end_, pos_;  
  int player_;
  int sound_;
  CTableView* pView_;
};

}

#endif