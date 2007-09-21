#ifndef __table_pduactionrequest_h__
#define __table_pduactionrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionRequest
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to request
             for an action. The spotlight player will respond with an
             action PDU.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"

class CTable;
class CPlayer;
class CChips;

class CpduActionRequest : public Cpdu
{
public:
  CpduActionRequest(CTable* table);
  int sendActionRequest(CPlayer* player,
                        const CChips& toPay,
                        const CChips& raise,
                        u_int16_t responseTime,
                        u_int16_t sequenceNumber);

private:
  CTable* table_;
};


#endif