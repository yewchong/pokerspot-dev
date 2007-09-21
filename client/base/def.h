#ifndef __base_def_h__
#define __base_def_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : Namespace Base
  Created  : 25.10.1999

  OVERVIEW : Basic type definitions.

 ****************************************************************************/


#include <list>
#include "base/card.h"

namespace Base
{
  const int CardHeight = 96;
  const int CardWidth = 70;

  typedef std::list<Card> Deck;
}

inline
ULONG make_rgba(BYTE r, BYTE g, BYTE b, BYTE a)
{
  return ULONG((r << 24) + (g << 16) + (b << 8) + a);
}

inline
void take_rgba(ULONG rgba, BYTE& r, BYTE& g, BYTE& b, BYTE& a)
{
  r = rgba >> 24;
  g = ((rgba & 0x00ff0000) >> 16);
  b = ((rgba & 0x0000ff00) >> 8);
  a = ((rgba & 0x000000ff));
}

#endif