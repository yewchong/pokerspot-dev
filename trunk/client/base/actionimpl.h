#ifndef __base_actionimpl_h__
#define __base_actionimpl_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : actionimpl.h
  Created  : 02/13/2000

  OVERVIEW : Implementations for the Ante, Call, Raise and Fold
             actions.

             The functions implement the visualization for
             these actions. There is some logic as well. The
             functions update the players' chips based on the
             action. The real money is of course moved only
             in the server side.

 ****************************************************************************/

class CChips;

namespace Base
{
  class Player;

  extern void ImplementAnte(Player*, int slot, const CChips& amount);
  extern void ImplementCall(Player*, int slot, const CChips& amount);
  extern void ImplementRaise(Player*, int slot, const CChips& bet, const CChips& raise);
  extern void ImplementFold(Player*, int slot);
  extern void ImplementSitOut(USHORT slot, Player*);
};

#endif