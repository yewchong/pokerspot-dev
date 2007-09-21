#include "pdugamenumber.h"
#include "../table/table.h"
#include "../player/player.h"


CpduGameNumber::CpduGameNumber()
{
    type_ = PDU_GameNumber;
    length_ = PDU_HEADER_SIZE + sizeof(u_int32_t);
    stuffHeader();
}


int CpduGameNumber::SendGameNumber(CTable* table)
{
    CpduGameNumber pdu;
    return pdu.sendGameNumber(table, table->getGameNumber());
}


int CpduGameNumber::sendGameNumber(CTable* table, u_int32_t number)
{
    number = htonl(number);
    memcpy(value_ + PDU_HEADER_SIZE, &number, sizeof(number));

    return broadcastTo(table->getRing());
}