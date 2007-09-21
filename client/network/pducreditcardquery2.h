#ifndef __network_pducreditcardquery2_h__
#define __network_pducreditcardquery2_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUCreditCardQuery2
  Created  : 04/24/2001

  OVERVIEW : Extended Credit Card Query PDU; sent by the lounge client
             to ask the server for player's credit cards for editing.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUCreditCardQuery2 : public PDUHeader
{
  // originated from client

  u_int16_t types_; // 1 => return cc types too

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++         = htons(types_);
    return (u_byte_t*)pint;
  }

  PDUCreditCardQuery2(bool typesToo)
    :
    PDUHeader(PDU_Lounge_CreditCardQuery2,
              sizeOf(), NULL, NULL),
    types_(typesToo ? 1 : 0)
  {}
};


END_NAMESPACE(Network)

#endif