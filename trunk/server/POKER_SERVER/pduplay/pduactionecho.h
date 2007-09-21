#ifndef __table_pduactionecho_h__
#define __table_pduactionecho_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionEcho
  Created  : 1/1/2001

  OVERVIEW : The table server sends this PDU to the clients to echo
             an action of one client.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"

class CTable;
class CPlayer;
class CChips;

class CpduActionEcho : public Cpdu
{
public:
  CpduActionEcho(CTable* table);
  int sendActionEcho(CPlayer* player,
                     u_int16_t action,
                     const CChips& amount);

private:
  CTable* table_;
};



#endif