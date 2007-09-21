#ifndef __lounge_ptr_h__
#define __lounge_ptr_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class Ptr
  Created  : 04/16/2000

  OVERVIEW : Semi-smart pointer class. The CPlayer and CTable objects
             are maintained in primary containers. Other places have
             containers of PlayerPtrs and TablePtrs that refer to objects
             in the primary containers. This avoids duplication of CPlayer
             and CTable objects.

 ****************************************************************************/

#include "lounge/playerptr.h"
#include "lounge/tableptr.h"


#endif
