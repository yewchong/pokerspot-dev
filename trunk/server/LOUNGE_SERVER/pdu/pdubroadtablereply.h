#ifndef __lounge_pdubroadtablereply_h__
#define __lounge_pdubroadtablereply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableReply
  Created  : 04/24/2000

  OVERVIEW : Receive and send Broad Table Reply PDU.

             Table server sends this pdu as a reply to
             Broad Table Query pdu. Lounge server forwards
             the reply to the player who originated the query-

 ****************************************************************************/

#include "pdu.h"
#include "compat.h"
#include "common/chips.h"


// The server does not interpret the data
// in any way but forwards it to the player.

#pragma NETWORK_ALIGN_BEGIN
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
  char username_[PDU_STRINGSIZE];
  char homecity_[PDU_STRINGSIZE];
  CChips::chip_amount_t chips_;
};
#pragma NETWORK_ALIGN_END


class CpduBroadTableReply : public Cpdu
{
public:
    CpduBroadTableReply();
    void grok(SOCKET sd, CTable*);

    u_int16_t getPlayerSocket() const;
};

inline u_int16_t CpduBroadTableReply::getPlayerSocket() const
{
    pdu_broadtablereply* pdu = (pdu_broadtablereply*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->playersocket);
}

#endif