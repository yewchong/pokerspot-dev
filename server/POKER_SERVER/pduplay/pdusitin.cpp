#include "pdusitin.h"
#include "../table/table.h"
#include "../player/player.h"

REGISTER_INCOMING_PDU(PDU_Play_SitIn, CpduSitIn);

CpduSitIn::CpduSitIn()
{}

void CpduSitIn::grok(SOCKET sd, CTable* table)
{
    CPlayer* player = table->getPlayerRawSocket(sd);

    if (player)
    {
        printf("Player %s sits in\n", player->getUsername());
        player->setSitIn(true);
    }

    // this pdu is done
    delete this;
}