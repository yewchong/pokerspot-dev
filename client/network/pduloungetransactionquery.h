#ifndef __network_pduloungetransactionquery_h__
#define __network_pduloungetransactionquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDULoungeCashOutRequest
  Created  : 05/16/2000

  OVERVIEW : Lounge Transaction Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDULoungeTransactionQuery : public PDUHeader
{
  // originated by client

  PDULoungeTransactionQuery()
    :
    PDUHeader(PDU_Lounge_TransactionQuery,
              sizeOf(), NULL, NULL)
  {}

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Lounge Transaction Query PDU ---");
    return PDUHeader::write(buf);
  }
};


END_NAMESPACE(Network)

#endif