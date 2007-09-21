/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Base::ChipAnimate
  Created  : 03/03/2000

  OVERVIEW : A class for doing chips animation.

 ****************************************************************************/

#include "stdafx.h"
#include "base/chipsanimate.h"
#include "base/graphic_chips.h"
#include "base/player.h"
#include "ui/tableview.h"
#include "ui/registry.h"
#include "ui/global.h"
#include "ui/soundmanager.h"
#include "tem/community.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace Base
{

ChipsAnimate::ChipsAnimate()
  :
  ticks_(0),
  msecs_(0),
  start_(0, 0),
  end_  (0, 0),
  pos_  (0, 0),
  player_(-1),
  sound_(-1),
  pView_(NULL)
{}


void ChipsAnimate::startAnimate(int from, int to,
                                const CChips& chips,
                                int sound)
{
  stopAnimate();

  sound_ = sound;

  CTableView* pV = CTableView::Instance();
  pView_ = pV;
  ASSERT(pView_);

  chips_ = chips;
  Player* pFrom = pV->getPlayer(from);
  Player* pTo = pV->getPlayer(to);
  if (pFrom && pTo)
  {
    player_ = from;
    msecs_ = Global::GetAnimationTime() / 2;
    ticks_ = 0;
    // Animation does not go to the pot right away,
    // chips move to the pot only after betting round
    // ends.
    start_ = pFrom->getChipStartPos();
    end_ = pFrom->getChipBetPos();
    //end_ = pTo->getChipPos();
    pos_ = start_;

    CDC* pDC = pView_->GetDC();
    if (pDC)
    {
      Chips::Instance().startAnimate(pDC, chips_, start_);
      pView_->ReleaseDC(pDC);
    }

    // Fix up the 'end' position so the bottom left corner
    // of the rectangle will hit there
    CSize s = Chips::Instance().getAnimationBufferSize();
    end_.y -= (s.cy - Chips::Height_);
  }
}


void ChipsAnimate::stopAnimate()
{
  if (msecs_ != 0)
  {
    if (sound_ >= 0 && sound_ < SndMan::SM_LastSound)
      SndMan::Instance()->playSound((SndMan::Sound)sound_);
    sound_ = -1;

    CDC* pDC = pView_->GetDC();
    if (pDC)
    {
      Chips::Instance().stopAnimate(pDC, pos_);
      pView_->ReleaseDC(pDC);
    }

    // Make sure all garbage is cleared from screen
    pView_->InvalidateRect(CRect(pos_,
                                 Chips::Instance().getAnimationBufferSize()));

    Player* player = pView_->getPlayer(player_);
    if (player)
    {
      player->setBetChips(player->getBetChips() + chips_);
    }
  }

  ticks_ = 0;
  msecs_ = 0;
}


BOOL ChipsAnimate::stepAnimate(DWORD dt)
{
  if (msecs_ == 0)
    return FALSE;

/*
  if (Registry::GetInstanceNumber() != 1)
  {
    stopAnimate();
    return FALSE;
  }
*/

  BOOL rc = TRUE;

  ticks_ += dt;

  if (ticks_ < msecs_)
  { //
    // Do a piece of the animation
    //
    CPoint diff = end_ - start_;
    float d = float(ticks_) / float(msecs_);
    diff.x *= d;
    diff.y *= d;
    CPoint newPos = start_ + diff;

    animate(newPos, pos_);
 
    pos_ = newPos;
  }
  else 
  { // 
    // Animation is complete
    //
    stopAnimate();
    rc = FALSE;
  }

  return rc;
}


void ChipsAnimate::animate(const CPoint& newPos,
                           const CPoint& oldPos)
{
  CDC* pDC = pView_->GetDC();
  if (pDC)
  {
    Chips::Instance().animate(pDC, chips_,
                              newPos, oldPos);
    pView_->ReleaseDC(pDC);
  }
}


} // Base