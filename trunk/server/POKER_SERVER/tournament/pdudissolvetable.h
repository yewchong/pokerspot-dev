#ifndef __table_pdudissolvetable_h__
#define __table_pdudissolvetable_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduDissolveTable
  Created  : 07/10/2000

  OVERVIEW : Send-only tournament dissolve table pdu.
             Lounge server tells the table server that it is
             about to be dissolved with this pdu.
             The table replies with a Dissolve Information PDU
             specifying current players and their chips. Lounge
             server then proceeds to reseat the players to other
             tables.

 ****************************************************************************/

#include "pduhandler/pduhandler.h"


class CpduDissolveTable : public Cpdu
{
public:
    CpduDissolveTable();
    void grok(SOCKET sd, CTable* table);
};


#endif
