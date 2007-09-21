#include "pdudealcardsflex.h"


CpduDealCardsFlex::CpduDealCardsFlex(void)
{
    type_ = PDU_Play_DealCardsFlex;
    length_ = PDU_HEADER_SIZE + sizeof(dealcardsflex_pdu);
};

void CpduDealCardsFlex::setDeal(u_int16_t slot, enum DealCardsType type)
{
    dealcardsflex_pdu *pPdu;
    
    pPdu = (struct dealcardsflex_pdu *) (value_ + PDU_HEADER_SIZE);

    pPdu->slot = htons(slot);
    pPdu->type = htons((u_int16_t)type);
    pPdu->numcards = 0;
};

void CpduDealCardsFlex::addCard(u_int16_t value, u_int16_t suit)
{
    dealcardsflex_pdu *pPdu;
    dealcardsflex_cardinfo *pCardInfo;


    pPdu = (struct dealcardsflex_pdu *) (value_ + PDU_HEADER_SIZE);
//    pCardInfo = (struct dealcardsflex_cardinfo *) (pPdu + sizeof(dealcardsflex_pdu) + (ntohs(pPdu->numcards) * sizeof(dealcardsflex_cardinfo)));
    pCardInfo = (dealcardsflex_cardinfo*) (value_ + PDU_HEADER_SIZE + sizeof(dealcardsflex_pdu) + (ntohs(pPdu->numcards) * sizeof(dealcardsflex_cardinfo)));

    pCardInfo->value = htons(value);
    pCardInfo->suit = htons(suit);

    pPdu->numcards = htons(ntohs(pPdu->numcards) + 1);
    length_ += sizeof(dealcardsflex_cardinfo);
};

int CpduDealCardsFlex::sendDealCards(CPlayer *player)
{
    stuffHeader();

    return sendTo(player->getConnfd());
};

int CpduDealCardsFlex::sendDummyDealCards(CPlayer* player, int numCards)
{
    setDeal(0xffff, Dummy);
    dealcardsflex_pdu* pPdu = (dealcardsflex_pdu*)(value_ + PDU_HEADER_SIZE);
    pPdu->numcards = htons(numCards);

    stuffHeader();
    return sendTo(player->getConnfd());
}

int CpduDealCardsFlex::sendAllDealCards(Cring* ring) // XXX 7-stud
{
    stuffHeader();
    return broadcastTo(ring);
}
