#include "pdudealcards.h"

CpduDealCards::CpduDealCards(void)
{
        type_ = PDU_Play_DealCards;
        length_ = PDU_HEADER_SIZE + sizeof(dealcards_pdu);
        stuffHeader();
};

int CpduDealCards::sendDealCards(CPlayer *player,
                                 u_int16_t dealer,
                                 u_int16_t card1,
                                 u_int16_t suit1,
                                 u_int16_t card2,
                                 u_int16_t suit2,
                                 u_int16_t msecs)
{
    dealcards_pdu pdu;
    memset(&pdu, 0, sizeof(pdu));
    pdu.dealer = htons(dealer);
    pdu.card1 = htons(card1);
    pdu.suit1 = htons(suit1);
    pdu.card2 = htons(card2);
    pdu.suit2 = htons(suit2);
    pdu.msecs = htons(msecs);


    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(player->getConnfd());
};
