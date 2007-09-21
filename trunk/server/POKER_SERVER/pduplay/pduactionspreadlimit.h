#ifndef __table_pduactionspreadlimit_h__
#define __table_pduactionspreadlimit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduActionSpreadLimit
  Created  : 1/1/2001

  OVERVIEW : The table client sends this PDU to the server to tell
             which action the player took.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"
#include "common/chips.h"


#pragma NETWORK_ALIGN_BEGIN
struct pdu_actionspreadlimit
{
  u_int16_t action;
  CChips::chip_amount_t raise;
  u_int16_t sequencenumber;
};
#pragma NETWORK_ALIGN_END


class CpduActionSpreadLimit : public Cpdu
{
public:
  CpduActionSpreadLimit();

  u_int16_t getAction() const;
  CChips getRaise() const;
  u_int16_t getSequenceNumber() const;
};

inline u_int16_t CpduActionSpreadLimit::getAction() const
{
  pdu_actionspreadlimit* pdu = (pdu_actionspreadlimit*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->action);
}

inline CChips CpduActionSpreadLimit::getRaise() const
{
  pdu_actionspreadlimit* pdu = (pdu_actionspreadlimit*)(value_ + PDU_HEADER_SIZE);
  return CChips(CChips_n(pdu->raise).ntohl());
}

inline u_int16_t CpduActionSpreadLimit::getSequenceNumber() const
{
  pdu_actionspreadlimit* pdu = (pdu_actionspreadlimit*)(value_ + PDU_HEADER_SIZE);
  return ntohs(pdu->sequencenumber);
}



#endif