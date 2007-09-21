#ifndef __lounge_pducreditcardquery_h__
#define __lounge_pducreditcardquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardQuery
  Created  : 04/19/2000

  OVERVIEW : Receive-only credit card query PDU - player wants to know the
             credit cards.

 ****************************************************************************/

#include "pdu.h"


class CpduCreditCardQuery : public Cpdu
{
public:
    CpduCreditCardQuery();
    void grok(SOCKET, CPlayer*);
};

#endif
