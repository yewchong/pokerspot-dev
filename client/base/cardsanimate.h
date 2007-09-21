#ifndef __base_cardsanimate_h__
#define __base_cardsanimate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Base::CardsAnimate
  Created  : 04/27/2000

  OVERVIEW : A class for doing cards animation.

 ****************************************************************************/

class CTableView;

namespace Base
{

class CardsAnimate
{
public:
  CardsAnimate();

  void startAnimate(int fromIndex,
                    int toIndex,
                    int numCards,
                    UINT msecs);
  void stopAnimate();
  BOOL stepAnimate(DWORD dt);
  BOOL isAnimationOn() const;

private:
  void animate(const CPoint& newPos,
               const CPoint& oldPos);
  bool loadBitmaps(int numCards);

  CBitmap bmpImage_, bmpMask_;
  int width_, height_;
  DWORD  ticks_, msecs_;
  CPoint start_, end_, pos_;  
  CTableView* pView_;
};

inline BOOL CardsAnimate::isAnimationOn() const
{
  return msecs_ > ticks_;
}

}

#endif