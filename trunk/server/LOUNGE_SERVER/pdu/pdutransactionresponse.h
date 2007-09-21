#ifndef __lounge_pdutransactionresponse_h__
#define __lounge_pdutransactionresponse_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTransactionResponse
  Created  : 05/26/2000

  OVERVIEW : Send-only Transaction Response PDU.

 ****************************************************************************/


#include "pdu.h"

class CpduTransactionResponse : public Cpdu
{
public:
    CpduTransactionResponse();
    int sendResponse(SOCKET sd, const char* msg);
};


#endif