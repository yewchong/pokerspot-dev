#ifndef __network_pdubalancequery_h__
#define __network_pdubalancequery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUBalanceQuery
  Created  : 04/19/2000

  OVERVIEW : Balance Query PDU; sent by the lounge client
             to ask the server for player's account balance.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUBalanceQuery : public PDUHeader
{
  // originated by client

  PDUBalanceQuery()
    :
    PDUHeader(PDU_Lounge_BalanceQuery,
              sizeOf(), NULL, NULL)
  {}

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Account Balance Query PDU ---");
    return PDUHeader::write(buf);
  }
};


END_NAMESPACE(Network)

#endif