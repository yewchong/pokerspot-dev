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

#include "tournament/pdudissolvetable.h"


CpduDissolveTable::CpduDissolveTable()
{
    type_ = PDU_Tournament_DissolveTable;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}
