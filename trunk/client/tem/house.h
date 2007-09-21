#ifndef __tem_house_h__
#define __tem_house_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class House
  Created  : 25.10.1999

  OVERVIEW : Implements House player for Texas Hold'em.

 ****************************************************************************/


#include "base/player.h"


class House : public Base::Player
{
public:
  House(PlayerSlot&);

  static BOOL LoadDeckBitmap(LPCTSTR pszImage,
                             LPCTSTR pszMask);

  // Player overrides
  BOOL drawImage(CDC* pDC, const CPoint& origo, DWORD flags) const;
  void mouseClick(const CPoint&);
  CPoint getCardPos(int index = Player::LastCard) const;
  CPoint getCardPos(const CPoint& origo, int index = Player::LastCard) const;
  CSize getSize() const;

private:
  CBitmap deckImage_;
  CBitmap deckMask_;
  static CBitmap BmpBuf_;
  int width_, height_;

  enum {
    NumDrawCards = 3,
    CardYOffset  = 10,
    CardXOffset  = 23
  };  
};


#endif