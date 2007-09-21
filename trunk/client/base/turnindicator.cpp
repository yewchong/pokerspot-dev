/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class TurnIndicator
  Created  : 12.11.1999

  OVERVIEW : Implementation of the "turn" pointer.

 ****************************************************************************/

#include "stdafx.h"
#include "base/turnindicator.h"
#include "base/player.h"
#include "base/draw.h"
#include "ui/global.h"
#include "ui/resource.h"
#include "ui/tableview.h"



namespace Base
{

TurnIndicator::TurnIndicator()
  :
  pView_      (CTableView::Instance()),
  waitTime_   (0),
  playerIndex_(-1),
  prevDir_    (PlayerSlot::TID_Left)
{
  timeText_.Format("%i", waitTime_ / 1000);

  bmpPointer_.LoadBitmap(IDB_ARROWRIGHT,
                         IDB_ARROWRIGHTMASK);

}


TurnIndicator::~TurnIndicator()
{
}


void TurnIndicator::tick(DWORD ticks)
{
  if (ticks < waitTime_ && playerIndex_ != -1) 
  {
    int secondsLeft = atoi((LPCSTR)timeText_);
    if (secondsLeft != ((waitTime_ - ticks) / 1000))
    { // Change seconds left text once per second
      timeText_.Format("%i", ((waitTime_ - ticks) / 1000));
      pView_->InvalidateRect(getPointerArea(playerIndex_));
    }
  }
}


void TurnIndicator::draw(CDC*  pDC)
{
  if (playerIndex_ != -1)
  {
    CRect rectPtr = getPointerArea(playerIndex_);
  
    if (pDC->RectVisible(rectPtr))
    { 
      bmpPointer_.TransBlt(pDC,
                           rectPtr.left,
                           rectPtr.top);

      // Paint the seconds left text with bold bump font
      CFont* pOldFont = pDC->SelectObject(Global::GetBoldFont());
      Base::DrawBumpText(pDC, timeText_, rectPtr, 
                         DT_CENTER|DT_SINGLELINE|DT_VCENTER,
                         RGB(255, 255, 255), RGB(0, 0, 0));
      pDC->SelectObject(pOldFont);
    }
  }
}


void TurnIndicator::setTurn(int playerIndex, DWORD waitTime)
{
  playerIndex_ = playerIndex;
  waitTime_ = waitTime;

  loadPointerBitmap(playerIndex_);
  pView_->InvalidateRect(getPointerArea(playerIndex_));
}


void TurnIndicator::endTurn()
{
  if (playerIndex_ != -1)
  {
    timeText_ = _T("0");
    waitTime_ = 0;
    if (pView_ && ::IsWindow(pView_->GetSafeHwnd()))
      pView_->InvalidateRect(getPointerArea(playerIndex_));
    playerIndex_ = -1;
  }
}


//
// Return the rectangle into which place the
// "your turn" pointer for player 'playerIndex'.
//
CRect TurnIndicator::getPointerArea(int playerIndex) const
{
  Player* pPlayer = pView_->getPlayer(playerIndex);
  if (pPlayer)
  {
    CRect r = pPlayer->getTextArea();
    CSize sizePointer = bmpPointer_.GetSize();

    const int Margin = 1;

    PlayerSlot::TurnIndicatorDirection d =
      pView_->playerTurnindicatorDir(playerIndex);

    if (d == PlayerSlot::TID_Bottom)
    { 
      return CRect(CPoint(r.left + r.Width() / 2 - sizePointer.cx / 2,
                          r.bottom + Margin),
                   sizePointer);
    }
    else if (d == PlayerSlot::TID_Left)
    {
      return CRect(CPoint(r.left - sizePointer.cx - Margin,
                          r.top + r.Height() / 2 - sizePointer.cy / 2),
                   sizePointer);
    }
    else if (d == PlayerSlot::TID_Top)
    {
      return CRect(CPoint(r.left + r.Width() / 2 - sizePointer.cx / 2,
                          r.top - sizePointer.cy - Margin),
                   sizePointer);
    }
    else // assume TID_Right
    {
      return CRect(CPoint(r.right,
                          r.top + r.Height() / 2 - sizePointer.cy / 2),
                   sizePointer);
    }
  }
  else
    return CRect(0, 0, 0, 0);
}
   

//
// Load the pointer bitmap anew if 'playerIndex'
// belongs to different player group than the
// previous player.
//
void TurnIndicator::loadPointerBitmap(int playerIndex)
{
  ASSERT(pView_);

  PlayerSlot::TurnIndicatorDirection d =
    pView_->playerTurnindicatorDir(playerIndex);
    
  if (d != prevDir_)
  {  
    prevDir_ = d;

    BOOL rc = FALSE;
    if (d == PlayerSlot::TID_Bottom)
    {
      bmpPointer_.DeleteObject();
      rc = bmpPointer_.LoadBitmap(IDB_ARROWUP,
                                  IDB_ARROWUPMASK);
      ASSERT(rc);
    }
    else if (d == PlayerSlot::TID_Left)
    {
      bmpPointer_.DeleteObject();
      rc = bmpPointer_.LoadBitmap(IDB_ARROWRIGHT,
                                  IDB_ARROWRIGHTMASK);
      ASSERT(rc);
    }
    else if (d == PlayerSlot::TID_Top)
    {
      bmpPointer_.DeleteObject();
      rc = bmpPointer_.LoadBitmap(IDB_ARROWDOWN,
                                  IDB_ARROWDOWNMASK);
      ASSERT(rc);
    }
    else // assume Right
    {
      bmpPointer_.DeleteObject();
      rc = bmpPointer_.LoadBitmap(IDB_ARROWLEFT,
                                  IDB_ARROWLEFTMASK);
      ASSERT(rc);
    }
  }
}

} // Base