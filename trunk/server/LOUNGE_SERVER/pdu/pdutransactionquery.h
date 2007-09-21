#ifndef __lounge_pdutransactionquery_h__
#define __lounge_pdutransactionquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTransactionQuery
  Created  : 05/26/2000

  OVERVIEW : Receive-only Transaction Query PDU.

 ****************************************************************************/


#include "pdu.h"

class CpduTransactionQuery : public Cpdu
{
public:
    CpduTransactionQuery();
    void grok(SOCKET, CPlayer*);
};


#endif