#ifndef __table_pduactionrequestspreadlimit_h__
#define __table_pduactionrequestspreadlimit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionRequestSpreadLimit
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to request
             for an action in a no-limit game. The spotlight player will
             respond with an action PDU.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"

class CTable;
class CPlayer;
class CChips;

class CpduActionRequestSpreadLimit : public Cpdu
{
public:
  CpduActionRequestSpreadLimit(CTable* table);
  int sendActionRequest(CPlayer* player,
                        const CChips& toPay,
                        const CChips& raiseLo,
                        const CChips& raiseHi,
                        u_int16_t responseTime,
                        u_int16_t sequenceNumber);

private:
  CTable* table_;
};


#endif