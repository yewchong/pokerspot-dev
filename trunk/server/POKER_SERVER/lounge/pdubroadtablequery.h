#ifndef __table_pdubroadtablequery_h__
#define __table_pdubroadtablequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableQuery
  Created  : 04/24/2000

  OVERVIEW : Receive-only Broad Table Query PDU.

             Player sends this pdu to the Lounge server
             which forwards it to the Table server.

 ****************************************************************************/

#include "../pduhandler/pduhandler.h"

struct pdu_broadtablequery
{
    u_int16_t playersocket;
};

class CpduBroadTableQuery : public Cpdu
{
public:
    CpduBroadTableQuery();
    void grok(SOCKET sd, CTable*);
    u_int16_t getPlayerSocket() const;
};

inline u_int16_t CpduBroadTableQuery::getPlayerSocket() const
{
    pdu_broadtablequery* pdu = (pdu_broadtablequery*)(value_ + PDU_HEADER_SIZE);
    return ntohs(pdu->playersocket);
}


#endif