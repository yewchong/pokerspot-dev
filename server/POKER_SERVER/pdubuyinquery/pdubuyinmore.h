#ifndef __table_pdubuyinmore_h__
#define __table_pdubuyinmore_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBuyInMore
  Created  : 05/11/2000

  OVERVIEW : Receive-only Buy-In More PDU - player wants to bring 
             more chips to the table.

 ****************************************************************************/


#include "../pduhandler/pduhandler.h"

class CpduBuyInMore : public Cpdu
{
public:
    CpduBuyInMore();
    void grok(SOCKET, CTable*);
};


#endif
