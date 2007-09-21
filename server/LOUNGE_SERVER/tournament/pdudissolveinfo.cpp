/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduDissolveInfo
  Created  : 07/10/2000

  OVERVIEW : 

 ****************************************************************************/

#include "tournament/pdudissolveinfo.h"
#include "tournament/basetournamentqueue.h"
#include "tournament/tournament.h"
#include "tournament/seater.h"
#include "sys.h"


REGISTER_INCOMING_PDU(PDU_Tournament_DissolveInfo, CpduDissolveInfo);

CpduDissolveInfo::CpduDissolveInfo()
{}

void CpduDissolveInfo::grok(SOCKET sd, CTable* table)
{
    // Reseat all players!
    int numPlayers = getNumPlayers();

    assert(sizeof(pdu_dissolveinfo) == 2);
    dissolve_playerinfo* dpi = (dissolve_playerinfo*)(value_ + PDU_HEADER_SIZE + sizeof(pdu_dissolveinfo));

    CBaseTournamentQueue* tq =
        CBaseTournamentQueue::FindQueue(table);
    if (!tq)
    {
        Sys_LogError("CpduDissolveInfo: No tournament queue found!\n");
        return;
    }

    CTournament* tournament = tq->findTournamentByTable(table->getNumber());
    if (!tournament)
    {
        Sys_LogError("CpduDissolveInfo: No tournament found!\n");
        return;
    }

    
    dissolve_playerinfo* d = dpi;
    for (int i = 0; i < numPlayers; i++, d++)
    {
        printf("Dissolve: %s %d\n", d->username_, ntohs(d->chips_));
    }


    CSeater s;
    s.createSeating(tq, tq->getTableList(),
                    table, dpi, numPlayers);

    // Commit the seating, players will be seated
    list<string> seated;
    s.commitSeating(tq, false, seated);

    CTournament* t = tq->getTournament();
    if (t)
        t->waitReseat(seated);

    // this pdu is done
    delete this;
}
