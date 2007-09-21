#ifndef __lounge_pducashoutrequest_h__
#define __lounge_pducashoutrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCashOutRequest
  Created  : 04/20/2000

  OVERVIEW : Receive-only cash out request PDU - player wants to cash out.

 ****************************************************************************/

#include "pdu.h"


class CpduCashoutRequest : public Cpdu
{
public:
    CpduCashoutRequest();
    void grok(SOCKET, CPlayer*);
    u_int16_t getCashoutAmount() const;
};

inline u_int16_t CpduCashoutRequest::getCashoutAmount() const
{
    u_int16_t amount = *((u_int16_t*)(value_ + PDU_HEADER_SIZE));
    return ntohs(amount);
}

#endif
