#ifndef __psi_animatewinner_h__
#define __psi_animatewinner_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : namespace AnimateWinner
  Created  : 02/08/2000

  OVERVIEW : Implements the winner animation.

 ****************************************************************************/

namespace AnimateWinner
{

BOOL StartAnimate(CWnd* pWnd, const CRect& playerArea, UINT msecs);
BOOL StopAnimate();

}

#endif
