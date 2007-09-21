#ifndef __base_parsemotd_h__
#define __base_parsemotd_h__


/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : parsemotd.h
  Created  : 01/25/2000

  OVERVIEW : MOTD & announce text parser & renderer

 ****************************************************************************/

namespace Base
{

extern
CRect SizeAnnounceText(CDC* pDC, const CString& text, const CRect& rect);

extern
void DrawAnnounceText(CDC* pDC, const CString& text, const CRect& rect);

}

#endif