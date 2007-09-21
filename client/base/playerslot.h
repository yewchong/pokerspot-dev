#ifndef __psi_playerslot_h__
#define __psi_playerslot_h__
/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : struct PlayerSlot
  Created  : 17.18.1999

  OVERVIEW : Each of the players have a slot at the table.
             The slot defines the player geometry such as
             label and card positions.

 ****************************************************************************/

namespace Base
{
  class Player;
}

//
// A PlayerSlot specifies the geometry how
// a player is visualized on the play surface.
// This geometry contains the label position,
// closed & open card positions and direction
// of the "Your Turn" indicator. The turn
// indicator is always positioned to point
// to the label.
//
struct PlayerSlot
{
  enum TurnIndicatorDirection
  {
    TID_Top    = 0,
    TID_Bottom = 1,
    TID_Left   = 2,
    TID_Right  = 3
  };

  enum ChipAnchor
  {
    CA_Left = 0,
    CA_Right = 1
  };

  TurnIndicatorDirection turnDir_;
  CPoint openCardPos_;
  CPoint backsidePos_;
  CPoint labelPos_;
  CPoint chipPos_;
  int    chipAnchor_;
  CPoint& pos_;
  Base::Player* pPlayer_;


  PlayerSlot()
    : turnDir_    (TID_Top),
      openCardPos_(0, 0),
      backsidePos_(0, 0),
      labelPos_   (0, 0),
      chipPos_    (0, 0),
      chipAnchor_ (CA_Left),
      pos_        (labelPos_),
      pPlayer_    (NULL)
  {}

  PlayerSlot(Base::Player* p)
    : turnDir_    (TID_Top),
      openCardPos_(0, 0),
      backsidePos_(0, 0),
      labelPos_   (0, 0),
      chipPos_    (0, 0),
      chipAnchor_ (CA_Left),
      pos_        (labelPos_),
      pPlayer_    (p)
  {}
  
  PlayerSlot(const PlayerSlot& p)
    : turnDir_    (p.turnDir_),
      openCardPos_(p.openCardPos_),
      backsidePos_(p.backsidePos_),
      labelPos_   (p.labelPos_),    
      chipPos_    (p.chipPos_),
      chipAnchor_ (p.chipAnchor_),
      pos_        (labelPos_),
      pPlayer_    (p.pPlayer_)
  {}

  PlayerSlot& operator = (const PlayerSlot& rhs)
  {
    if (this != &rhs)
    {
      turnDir_     = rhs.turnDir_;
      openCardPos_ = rhs.openCardPos_;
      backsidePos_ = rhs.backsidePos_;
      labelPos_    = rhs.labelPos_;
      chipPos_     = rhs.chipPos_;
      chipAnchor_  = rhs.chipAnchor_;
      pPlayer_     = rhs.pPlayer_;
    }
    return *this;
  }
};

#include <vector>
typedef std::vector<PlayerSlot> Players;


#endif