#ifndef __base_announcetext_h__
#define __base_announcetext_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : announcetext.h
  Created  : 01/26/2000

  OVERVIEW : Routines for drawing announcement text.

 ****************************************************************************/

namespace Base
{

extern
CRect SizeAnnounceText(CDC* pDC, const CString& text, const CRect& rect);

extern
void DrawAnnounceText(CDC* pDC, const CString& text, const CRect& rect);

}

#endif