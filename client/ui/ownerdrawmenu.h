#pragma once
#ifndef __psi_ownerdrawmenu_h__
#define __psi_ownerdrawmenu_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class COwnerDrawMenu
  Created  : 06/07/2000

  OVERVIEW : Class for owner drawn menus.

 ****************************************************************************/


class COwnerDrawMenu : public CMenu
{
public:
// Implementation
	virtual void MeasureItem(LPMEASUREITEMSTRUCT lpMIS);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDIS);
	COwnerDrawMenu();
	virtual ~COwnerDrawMenu();
};


#endif