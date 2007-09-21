#include "pdudealcommunity.h"


CpduDealCommunity::CpduDealCommunity(void)
{
    type_ = PDU_Play_CommunityCard;
    length_ = PDU_HEADER_SIZE + sizeof(dealcommunity_pdu);
    stuffHeader();
};

int CpduDealCommunity::sendDealCommunity(Cring *ring,
                                         u_int16_t value,
                                         u_int16_t suit,
                                         u_int16_t msecs)
{
    dealcommunity_pdu pdu;
    memset(&pdu, 0, sizeof(pdu));
    pdu.value = htons(value);
    pdu.suit = htons(suit);
    pdu.msecs = htons(msecs);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return broadcastTo(ring);
};