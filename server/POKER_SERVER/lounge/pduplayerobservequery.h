#ifndef __table_pduplayerobservequery_h__
#define __table_pduplayerobservequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveQuery
  Created  : 05/27/2000

  OVERVIEW : Receive-only Player Observe Query PDU - lounge server sends
             this pdu to a table server to ask if a player can observe
             the table.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"

struct pdu_playerobservequery
{
  u_int32_t ipaddress;
  char username[PDUSTRINGSIZE];
};

class CpduPlayerObserveQuery : public Cpdu
{
public:
  CpduPlayerObserveQuery();
  void grok(SOCKET sd, CTable*);

  u_int32_t getIpAddress() const;
  const char* getUsername() const;

  char sz_username_[PDUSTRINGSIZE+1];
};

inline u_int32_t CpduPlayerObserveQuery::getIpAddress() const
{
  pdu_playerobservequery* pdu = (pdu_playerobservequery*)(value_ + PDU_HEADER_SIZE);
  return ntohl(pdu->ipaddress);
}

inline const char* CpduPlayerObserveQuery::getUsername() const
{
  return sz_username_;
}


#endif