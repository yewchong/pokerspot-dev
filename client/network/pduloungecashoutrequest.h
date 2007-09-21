#ifndef __network_pduloungecashoutrequest_h__
#define __network_pduloungecashoutrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeCashOutRequest
  Created  : 03/13/2000

  OVERVIEW : Lounge Cash Out Request PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDULoungeCashOutRequest : public PDUHeader
{
  // originated by client

  CChips amount_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(CChips::chip_amount_t); }

  PDULoungeCashOutRequest(const CChips& amount)
    :
    PDUHeader  (PDU_Lounge_CashOutRequest,
                sizeOf(), NULL, NULL),
    amount_    (amount)
  {}

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Lounge Cash Out Request PDU ---");

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)PDUHeader::write(buf);
    *pchips++ = amount_.htonl_rep();

#ifdef PDU_DEBUG_
    CString s;
    s.Format("   amount: %f", amount_.asDouble());
    PDU_DEBUG(s);
#endif

    return (u_byte_t*)pchips;
  }
};


END_NAMESPACE(Network)

#endif