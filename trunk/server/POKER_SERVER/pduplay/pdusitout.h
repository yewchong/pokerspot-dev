#ifndef __table_pdusitout_h__
#define __table_pdusitout_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CPduSitout
  Created  : 1/1/2001

  OVERVIEW : The table client sends this PDU to the server to tell
             the player wants to sit out.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"

class CpduSitOut : public Cpdu
{
public:
  CpduSitOut();
  void grok(SOCKET, CTable*);
};

#endif