#ifndef __table_pduanteallin_h__
#define __table_pduanteallin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduAnteAllIn
  Created  : 08/13/2000

  OVERVIEW : This pdu is used to prepare the client to do an
             all-in on subsequent ante.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

class CTable;
class CChips;

class CpduAnteAllIn : public Cpdu
{
public:
  CpduAnteAllIn();
  int sendAllIn(CTable* table, const CChips& bet);
};


#endif