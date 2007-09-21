#ifndef __base_gamestate_h__
#define __base_gamestate_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::GameState
  Created  : 25.10.1999

  OVERVIEW : GameState is an abstract base class for game states.
             Derive concrete states from this class.

             AnimateState has ready-made functionality for card
             animation. Derive your state from AnimateState if it
             wants to do animation.             

 ****************************************************************************/


namespace Base
{

class GameState
{
public:
  GameState();
  virtual ~GameState();

  virtual BOOL tick     (DWORD dt) = 0;
  virtual BOOL draw     (CDC*) = 0;
  virtual BOOL onCommand(WPARAM, LPARAM);
  virtual BOOL onKey    (UINT nChar, UINT nRepCnt, UINT nFlags);
  virtual BOOL onWndMove(int, int);
};


class AnimateState : public GameState
{
public:
  AnimateState(CView*);

protected:
  void startAnimate();
  void stopAnimate();
  void animate(const CPoint& newPos,
               const CPoint& oldPos);

  DWORD   ticks_;
  CPoint  start_, end_, pos_;
  
  CView*  pView_;
};

} // Base


#endif