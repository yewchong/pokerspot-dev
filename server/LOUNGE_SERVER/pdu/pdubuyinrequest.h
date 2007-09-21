#ifndef __lounge_pdubuyinrequest_h__
#define __lounge_pdubuyinrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBuyinRequest
  Created  : 04/20/2000

  OVERVIEW : Receive-only buy in request PDU - player wants to buy in.

 ****************************************************************************/

#include "pdu.h"


class CpduBuyinRequest : public Cpdu
{
public:
    CpduBuyinRequest();
    void grok(SOCKET, CPlayer*);
    u_int16_t getCardIndex() const;
    u_int16_t getBuyinAmount() const;

};

inline u_int16_t CpduBuyinRequest::getCardIndex() const
{
    u_int16_t index = *((u_int16_t*)(value_ + PDU_HEADER_SIZE));
    return ntohs(index);
}

inline u_int16_t CpduBuyinRequest::getBuyinAmount() const
{
    u_int16_t amount = *((u_int16_t*)(value_ + PDU_HEADER_SIZE + sizeof(u_int16_t)));
    return ntohs(amount);
}

#endif
