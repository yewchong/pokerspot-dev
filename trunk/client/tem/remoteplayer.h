#ifndef __tem_remoteplayer_h__
#define __tem_remoteplayer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class RemotePlayer
  Created  : 25.10.1999

  OVERVIEW : Implements RemotePlayer player for Texas Hold'em.

 ****************************************************************************/


#include "base/player.h"

//
// This class represents a remote player.
//
class RemotePlayer : public Base::Player
{
public:
  RemotePlayer(PlayerSlot& slot, LPCSTR name, const CChips& credit);

  // Player overrides
  BOOL drawImage(CDC* pDC, const CPoint& origo, DWORD flags) const;
  void mouseClick(const CPoint&);
  void setShowAll(BOOL); 

private:
  BOOL showAll_;
};


#endif