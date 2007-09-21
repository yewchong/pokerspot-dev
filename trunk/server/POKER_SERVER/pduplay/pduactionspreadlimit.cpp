/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionSpreadLimit
  Created  : 1/1/2001

  OVERVIEW : The table client sends this PDU to the server to tell
             which action the player took.

 ****************************************************************************/

#include "pduactionspreadlimit.h"


REGISTER_INCOMING_PDU(PDU_Play_ActionSpreadLimit, CpduActionSpreadLimit);


CpduActionSpreadLimit::CpduActionSpreadLimit()
{}
