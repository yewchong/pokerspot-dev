#ifndef __lounge_pduplayerloginquery_h__
#define __lounge_pduplayerloginquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginQuery
  Created  : 04/24/2000

  OVERVIEW : Send-only Player Login Query PDU - lounge server sends
             this pdu to a table server to ask if a player can be
             seated in that table.

 ****************************************************************************/

#include "pdu.h"


class CTable;
class CPlayer;

class CpduPlayerLoginQuery : public Cpdu
{
public:
  CpduPlayerLoginQuery();
  int sendLoginQuery(CTable*, CPlayer*);
};


#endif