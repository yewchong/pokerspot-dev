#ifndef __base_turnindicator_h__
#define __base_turnindicator_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class TurnIndicator
  Created  : 12.11.1999

  OVERVIEW : Implementation of the "turn" pointer.

 ****************************************************************************/

#include "base/transparentbitmap.h"
#include "playerslot.h"

class CTableView;

namespace Base
{

class TurnIndicator
{
public:
  TurnIndicator();
  ~TurnIndicator();

  void tick(DWORD ticks);
  void draw(CDC*  pDC);
  void setTurn(int playerIndex, DWORD waitTime);
  void endTurn();

  CRect getPointerArea(int playerIndex) const;
  void loadPointerBitmap(int playerIndex);

private:
  CTableView* pView_;
  Base::TransparentBitmap bmpPointer_;
  DWORD waitTime_;
  int   playerIndex_;
  PlayerSlot::TurnIndicatorDirection prevDir_;
  CString timeText_;
};


} // Base


#endif