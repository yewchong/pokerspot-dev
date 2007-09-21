#ifndef __table_pduplayerloginquery_h__
#define __table_pduplayerloginquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginQuery
  Created  : 04/24/2000

  OVERVIEW : Receive-only Player Login Query PDU - lounge server sends
             this pdu to a table server to ask if a player can be
             seated in that table.

 ****************************************************************************/

#include "../pduhandler/pduhandler.h"

struct pdu_playerloginquery
{
  u_int32_t ipaddress;
  char username[PDUSTRINGSIZE];
};

class CpduPlayerLoginQuery : public Cpdu
{
public:
  CpduPlayerLoginQuery();
  void grok(SOCKET sd, CTable*);

  u_int32_t getIpAddress() const;
  const char* getUsername() const;

  char sz_username[PDUSTRINGSIZE+1];
};

inline u_int32_t CpduPlayerLoginQuery::getIpAddress() const
{
  pdu_playerloginquery* pdu = (pdu_playerloginquery*)(value_ + PDU_HEADER_SIZE);
  return ntohl(pdu->ipaddress);
}

inline const char* CpduPlayerLoginQuery::getUsername() const
{
  return sz_username;
}


#endif