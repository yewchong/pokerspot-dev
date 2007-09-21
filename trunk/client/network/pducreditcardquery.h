#ifndef __network_pducreditcardquery_h__
#define __network_pducreditcardquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUCreditCardQuery
  Created  : 03/13/2000

  OVERVIEW : Credit Card Query PDU; sent by the lounge client
             to ask the server for player's credit cards.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUCreditCardQuery : public PDUHeader
{
  // originated by client

  PDUCreditCardQuery ()
    :
    PDUHeader(PDU_Lounge_CreditCardQuery,
              sizeOf(), NULL, NULL)
  {}

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Credit Card Query PDU ---");
    return PDUHeader::write(buf);
  }
};


END_NAMESPACE(Network)

#endif