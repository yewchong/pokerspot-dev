#include "pduping.h"
#include "lounge/player.h"


CpduPing::CpduPing()
{
    type_ = PDU_Ping;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_ping);
    stuffHeader();
}


int CpduPing::sendPing(CPlayer* player, long now)
{
    pdu_ping pdu;
    memset(&pdu, 0, sizeof(pdu));
    pdu.data = htonl(now);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(player->getSocket());
}
