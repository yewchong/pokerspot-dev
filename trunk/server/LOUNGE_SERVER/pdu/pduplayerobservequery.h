#ifndef __lounge_pduplayerobservequery_h__
#define __lounge_pduplayerobservequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveQuery
  Created  : 04/25/2000

  OVERVIEW : Send-only Player Observe Query PDU - lounge server sends
             this pdu to a table server to ask if a player can observe
             a table.

 ****************************************************************************/

#include "pdu.h"


class CTable;
class CPlayer;

class CpduPlayerObserveQuery : public Cpdu
{
public:
  CpduPlayerObserveQuery();
  int sendObserveQuery(CTable*, CPlayer*);
};


#endif