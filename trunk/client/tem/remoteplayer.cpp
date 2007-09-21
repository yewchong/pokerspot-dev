/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class RemotePlayer
  Created  : 25.10.1999

  OVERVIEW : Implements RemotePlayer player for Texas Hold'em.

 ****************************************************************************/

#include "stdafx.h"
#include "tem/remoteplayer.h"
#include "base/cards.h"
#include "base/registry.h"
#include "base/draw.h"
#include "ui/registry.h"
#include "ui/global.h"



RemotePlayer::RemotePlayer(PlayerSlot& slot, LPCTSTR name, const CChips& credit)
  :
  Base::Player(slot, name, credit),
  showAll_    (FALSE)
{
  if (Global::GetGameType() == GT_SevenStud)
  { // Cards are drawn as large
    showAll_ = TRUE;
  }
}


//
// Draw the backsides of cards.
//
BOOL RemotePlayer::drawImage(CDC* pDC, const CPoint& origo, DWORD dwFlags) const
{ 
  DWORD flags = dwFlags;
  if (!showAll_)
  { // By default, remote players don't show cards
    flags |= Base::Cards::C_BackSide;
  }

  return Player::drawImage(pDC, origo, flags);
}


void RemotePlayer::mouseClick(const CPoint&)
{ // Override to do nothing
}


void RemotePlayer::setShowAll(BOOL b)
{
  if (Global::GetGameType() != GT_SevenStud) 
  { // 7-stud draws cards always as large
    showAll_ = b;
    drawBackside_ = !b;
  }
}