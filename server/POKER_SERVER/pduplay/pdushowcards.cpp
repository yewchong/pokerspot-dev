/*******************************************************************************

    NAME:       pduShowCards
    DATE:       4/16/00
    PURPOSE:    Receive-only ShowCards pdu

*******************************************************************************/


#include "pdushowcards.h"
#include "../table/table.h"
#include "../player/player.h"

REGISTER_INCOMING_PDU(PDU_Play_ShowCards, CpduShowCards);

CpduShowCards::CpduShowCards()
{}

void CpduShowCards::grok(SOCKET sd, CTable* table)
{
    CPlayer* player = table->getPlayerRawSocket(sd);
    if (player)
    {
        bool muckHand = !getShowCards();
        printf("Player %s mucks hand: %d\n",
               player->getUsername(), muckHand);
        player->setMuckCards(muckHand);
    }

    // this pdu is done
    delete this;
}
