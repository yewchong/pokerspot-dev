#ifndef __tem_playerindex_h__
#define __tem_playerindex_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : namespace PlayerIndex
  Created  : 26.10.1999

  OVERVIEW : Player indices for Texas Hold'em. The main view
             stores players in this order.

 ****************************************************************************/



enum PlayerIndex
{
  PI_NULL        = -4,
  PI_House       = -3,
  PI_Community   = -2,
  PI_LocalPlayer = -1
  // rest are remote players
};


#endif

