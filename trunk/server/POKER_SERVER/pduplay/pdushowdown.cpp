/*******************************************************************************

    NAME:       pduShowdown
    DATE:       4/16/00
    PURPOSE:    Send-only Showdown pdu

*******************************************************************************/

#include "pdushowdown.h"
#include "pdudealcardsflex.h"
#include "../player/player.h"
#include "../table/table.h"
#include "../hand/hand.h"
#include "../deck/card.h"

struct pdu_showdown
{
    u_int16_t slot;
    u_int16_t numCards;
    u_int16_t msglen;
};

struct pdu_card
{
    u_int16_t suit;
    u_int16_t value;
};


CpduShowdown::CpduShowdown(CTable* table)
    :
    table_(table)
{
    type_ = PDU_Play_Showdown;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_showdown);
    stuffHeader();
}

int CpduShowdown::sendShowdown(CPlayer* player,
                               int numCards,
                               const char* message)
{
    // Sending dummy cards for now!
    
    CCard *pCard = player->getHand()->getCard();
    int msglen = (message != NULL ? strlen(message) : 0);

    length_ += numCards * sizeof(pdu_card) + msglen;
    stuffHeader();

    if (length_ < MAXBUFFERSIZE)
    {
        char* buf = value_ + PDU_HEADER_SIZE;
        pdu_showdown data;

        data.slot = htons(player->getSlot());
        data.numCards = htons(numCards);
        data.msglen = htons(msglen);
        memcpy(buf, &data, sizeof(data));
        buf += sizeof(data);
    
        // copy cards to buffer
        for (int i = 0; i < numCards; i++)
        {
            pdu_card card;
            card.suit = htons(pCard->getSuit());
            card.value = htons(pCard->getValue());
            memcpy(buf, &card, sizeof(card));
            buf += sizeof(card);
            pCard = pCard->getNext();
        }

        // copy showdown message to buffer
        if (msglen > 0)
            memcpy(buf, message, msglen);

        return broadcastTo(table_->getRing());
    }
    else
    {
        Sys_LogError("CpduShowdown: buffer size too small.\n");
        return 0;
    }
}

