#ifndef __lounge_pduplayerloginreply_h__
#define __lounge_pduplayerloginreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerLoginReply
  Created  : 04/24/2000

  OVERVIEW : Receive-only Player Login Reply PDU - table server replies
             whether it is ok to seat a player in the table.

 ****************************************************************************/

#include "pdu.h"

enum LoginReply
{
  LR_Reject = 0,
  LR_Accept = 1
};
    
struct pdu_playerloginreply
{
  u_int16_t tablenumber;
  char username[PDU_STRINGSIZE];
  u_int16_t reply;
};

class CpduPlayerLoginReply : public Cpdu
{
public:
  CpduPlayerLoginReply();
  void grok(SOCKET sd, CTable*);

  u_int16_t getReply() const;
  u_int16_t getTableNumber() const;
};

inline u_int16_t CpduPlayerLoginReply::getReply() const
{
  pdu_playerloginreply* pdu = (pdu_playerloginreply*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->reply);
}

inline u_int16_t CpduPlayerLoginReply::getTableNumber() const
{
  pdu_playerloginreply* pdu = (pdu_playerloginreply*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->tablenumber);
}


#endif