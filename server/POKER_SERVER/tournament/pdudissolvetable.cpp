/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduDissolveTable
  Created  : 07/10/2000

  OVERVIEW : Receive-only tournament dissolve table pdu.
             Lounge server tells the table server that it is
             about to be dissolved with this pdu.
             The table replies with a Dissolve Information PDU
             specifying current players and their chips. Lounge
             server then proceeds to reseat the players to other
             tables.

 ****************************************************************************/

#include "tournament/pdudissolvetable.h"
#include "tournament/tournament.h"
#include "table/table.h"


REGISTER_INCOMING_PDU(PDU_Tournament_DissolveTable, CpduDissolveTable);

CpduDissolveTable::CpduDissolveTable()
{}


void CpduDissolveTable::grok(SOCKET sd, CTable* table)
{
    char buf[100];
    sprintf(buf, "TOURNAMENT: CpduDissolveTable %i\n", table->getSeed());
    Sys_LogError(buf);

    CTournament::Inst()->setDissolve(true);

    // This pdu is done
    delete this;
}