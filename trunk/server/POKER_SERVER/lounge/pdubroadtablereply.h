#ifndef __table_pdubroadtablereply_h__
#define __table_pdubroadtablereply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableReply
  Created  : 04/24/2000

  OVERVIEW : Send-only Broad Table Reply PDU.

             Table server sends this pdu as a reply to
             Broad Table Query pdu. Lounge server forwards
             the reply to the player who originated the query.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"
#include "common/chips.h"

#pragma NETWORK_ALIGN_BEGIN
// This comes from the table
struct pdu_broadtablereply
{
  u_int16_t playersocket;
  CChips::chip_amount_t avg_pot_size;
  u_int16_t flop_prct;
  u_int16_t num_players;
};

// 'num_players_' times:
struct playerinfo
{
  char username[PDUSTRINGSIZE];
  char homecity[PDUSTRINGSIZE];
  CChips::chip_amount_t chips;
};
#pragma NETWORK_ALIGN_END

class CpduBroadTableReply : public Cpdu
{
public:
  CpduBroadTableReply();
  int sendTableReply(CTable*, u_int16_t tableSocket, u_int16_t playerSocket);
};


#endif