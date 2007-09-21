#ifndef __table_pdublindrequest_h__
#define __table_pdublindrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduBlindRequest
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to prompt
             for a player to post the blind.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"

class CTable;
class CPlayer;
class CChips;

class CpduBlindRequest : public Cpdu
{
public:
  CpduBlindRequest(CTable* table);
  int sendBlindRequest(CPlayer*,
                       const CChips& value,
                       u_int16_t responseTime,
                       u_int16_t sequenceNumber);
private:
  CTable* table_;
};


#endif