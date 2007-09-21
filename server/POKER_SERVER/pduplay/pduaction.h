#ifndef __table_pduaction_h__
#define __table_pduaction_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduAction
  Created  : 1/1/2001

  OVERVIEW : The table client sends this PDU to the server to tell which
             action the player took.

 ****************************************************************************/



#include "pduhandler/pduhandler.h"

struct pdu_action
{
  u_int16_t action;
  u_int16_t sequencenumber;
};

class CpduAction : public Cpdu
{
public:
    CpduAction();

    u_int16_t getAction() const;
    u_int16_t getSequenceNumber() const;
};

inline u_int16_t CpduAction::getAction() const
{
  pdu_action* pdu = (pdu_action*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->action);
}

inline u_int16_t CpduAction::getSequenceNumber() const
{
  pdu_action* pdu = (pdu_action*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->sequencenumber);
}



#endif