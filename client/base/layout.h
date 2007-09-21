#ifndef __base_layout_h__
#define __base_layout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : layout.cpp
  Created  : 25.11.1999

  OVERVIEW : Implements loading of a table layout from a text file.

 ****************************************************************************/

class CTableView;


namespace Base
{


//
// Load player positions from file 'fname' and
// update 'pView' accordingly.
//
extern void LoadLayout(const char* fname, CTableView* pView);


};


#endif