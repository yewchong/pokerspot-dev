#ifndef __lounge_pdubroadtablequery_h__
#define __lounge_pdubroadtablequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableQuery
  Created  : 04/24/2000

  OVERVIEW : Receive and send Broad Table Query PDU.

             Player sends this pdu to the Lounge server
             which forwards it to the Table server.

 ****************************************************************************/

#include "pdu.h"

// This comes from the player
struct pdu_broadtablequery_player
{
  u_int16_t tablenumber;
};

// This is sent to the table
struct pdu_broadtablequery_table
{
  u_int16_t lounge;
  u_int16_t playersocket;
};


class CpduBroadTableQuery : public Cpdu
{
public:
  CpduBroadTableQuery();
  void grok(SOCKET sd, CPlayer*);
  u_int16_t getTableNumber() const;
};

inline u_int16_t CpduBroadTableQuery::getTableNumber() const
{
  pdu_broadtablequery_player* pdu = (pdu_broadtablequery_player*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->tablenumber);
}


#endif