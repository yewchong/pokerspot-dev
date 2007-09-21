#ifndef __network_pduloungebuyinrequest_h__
#define __network_pduloungebuyinrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeBuyInRequest
  Created  : 03/13/2000

  OVERVIEW : Lounge Buy In Request PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"
#include "common/stringoutput.h"



BEGIN_NAMESPACE(Network)


struct PDULoungeBuyInRequest : public PDUHeader
{
  // originated by client

  u_int16_t card_index_;
  CChips    amount_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t); }

  PDULoungeBuyInRequest(u_int16_t card_index,
                        const CChips& amount)
    :
    PDUHeader  (PDU_Lounge_BuyInRequest,
                sizeOf(), NULL, NULL),
    card_index_(card_index),
    amount_    (amount)
  {}

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Lounge Buy In Request PDU ---");

    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(card_index_);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    *pchips++ = amount_.htonl_rep();

#ifdef PDU_DEBUG_
    CStrOut s;
    s << "   card: " << card_index_ << " amount: " << amount_;
    PDU_DEBUG(s.str());
#endif

    return (u_byte_t*)pchips;
  }
};


END_NAMESPACE(Network)

#endif