/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class Base::GameState
  Created  : 25.10.1999

  OVERVIEW : GameState is an abstract base class for game states.
             Derive concrete states from this class.

             AnimateState has ready-made functionality for animation.
             Derive your state from AnimateState if it has animation.             

 ****************************************************************************/


#include "stdafx.h"
#include "base/gamestate.h"
#include "base/cards.h"
#include "base/card.h"
#include "base/player.h"
#include "network/network.h"
#include "ui/tableview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{

inline bool IsVisible(CDC* pDC, const CRect& r)
{
  return pDC->PtVisible(r.TopLeft()) ||
         pDC->PtVisible(r.BottomRight());
}

inline bool IsClipped(CDC* pDC, const CRect& r)
{
  return !pDC->PtVisible(r.TopLeft()) ||
         !pDC->PtVisible(r.BottomRight());
}

} // namespace


namespace Base
{

//
// GameState
//
GameState::GameState()
{
}


GameState::~GameState()
{
}


BOOL GameState::onCommand(WPARAM wParam, LPARAM lParam)
{
  return FALSE;
}


BOOL GameState::onKey(UINT nChar, UINT nRepCnt, UINT nFlags)
{
  return FALSE;
}


BOOL GameState::onWndMove(int, int)
{
  return FALSE;
}


//
// AnimateState
//
AnimateState::AnimateState(CView* pView)
  :
  ticks_(0),
  start_(0, 0),
  end_  (0, 0),
  pos_  (0, 0),
  pView_(pView)
{
  ASSERT(pView_);
}


void AnimateState::startAnimate()
{
  CDC* pDC = pView_->GetDC();
  if (pDC)
  {
    Cards::Instance().startAnimate(pDC, start_);
    pView_->ReleaseDC(pDC);
  }
}


void AnimateState::stopAnimate()
{
  CDC* pDC = pView_->GetDC();
  if (pDC)
  {
    Cards::Instance().stopAnimate(pDC, pos_);
    pView_->ReleaseDC(pDC);
  }
}


void AnimateState::animate(const CPoint& newPos,
                           const CPoint& oldPos)
{
  static bool clipped = false;

  CDC* pDC = pView_->GetDC();
  if (pDC)
  {
    Cards::Instance().animate(pDC, Card(Base::Hearts, 1),
                              newPos, oldPos, Cards::C_BackSide);
    pView_->ReleaseDC(pDC);
  }
}


} // Base