#include "pduheartbeat.h"


REGISTER_INCOMING_PDU(PDU_HeartBeat, CpduHeartBeat);

CpduHeartBeat::CpduHeartBeat()
{}

void CpduHeartBeat::grok(SOCKET sd, CTable* table)
{
    // this pdu is done
    delete this;
}
