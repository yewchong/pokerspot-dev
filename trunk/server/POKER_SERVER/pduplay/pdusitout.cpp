#include "pdusitout.h"
#include "../table/table.h"
#include "../player/player.h"


REGISTER_INCOMING_PDU(PDU_Play_SitOut, CpduSitOut);

CpduSitOut::CpduSitOut()
{}

void CpduSitOut::grok(SOCKET sd, CTable* table)
{
    CPlayer* player = table->getPlayerRawSocket(sd);

    if (player)
    {
        printf("Player %s sits out\n", player->getUsername());
        player->setSitIn(false);
    }

    // this pdu is done
    delete this;
}