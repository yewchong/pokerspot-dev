#ifndef __network_pduactionspreadlimit_h__
#define __network_pduactionspreadlimit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUActionSpreadLimit
  Created  : 07/02/2000

  OVERVIEW : Spread limit Action PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"
#include "common/stringoutput.h"

BEGIN_NAMESPACE(Network)


struct PDUActionSpreadLimit : public PDUHeader
{
  u_int16_t action_;
  CChips    raiseAmount_; // if action raise
  u_int16_t sequencenumber_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t); }

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Action PDU ---");
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(action_);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    *pchips++ = raiseAmount_.htonl_rep();

    pint = (u_int16_t*)pchips;
    *pint++ = htons(sequencenumber_);

#ifdef PDU_DEBUG_
    CStrOut s;
    s << "   action: " << action_ << " raise: " << raiseAmount_;
    PDU_DEBUG(s.str());
#endif
    return (u_byte_t*)pint;
  }

  PDUActionSpreadLimit(const char* username,
                       const char* password,
                       u_int16_t   action,
                       const CChips& raise,
                       u_int16_t   sequencenumber)
    :
    PDUHeader   (PDU_Play_ActionSpreadLimit,
                 sizeOf(),
                 username, password),
    action_     (action),
    raiseAmount_(raise),
    sequencenumber_(sequencenumber)
  {}
};


END_NAMESPACE(Network)

#endif