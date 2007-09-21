#ifndef __lounge_pduplayerobservereply_h__
#define __lounge_pduplayerobservereply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerObserveReply
  Created  : 04/25/2000

  OVERVIEW : Receive-only Player Observe Reply PDU - table server replies
             whether it is ok to let a player observe a table.

 ****************************************************************************/

#include "pdu.h"

enum ObserveReply
{
  OR_Reject = 0,
  OR_Accept = 1
};
    

struct pdu_playerobservereply
{
  u_int16_t tablenumber;
  char username[PDU_STRINGSIZE];
  u_int16_t reply;
};


class CpduPlayerObserveReply : public Cpdu
{
public:
  CpduPlayerObserveReply();
  void grok(SOCKET sd, CTable*);

  u_int16_t getReply() const;
  u_int16_t getTableNumber() const;
};

inline u_int16_t CpduPlayerObserveReply::getReply() const
{
  pdu_playerobservereply* pdu = (pdu_playerobservereply*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->reply);
}

inline u_int16_t CpduPlayerObserveReply::getTableNumber() const
{
  pdu_playerobservereply* pdu = (pdu_playerobservereply*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->tablenumber);
}


#endif