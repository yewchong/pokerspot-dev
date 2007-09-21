/*******************************************************************************

    NAME:       pduShowCards
    DATE:       5/13/00
    PURPOSE:    Receive-only Handlog Request PDU

*******************************************************************************/

#include "pduhandlogrequest.h"
#include "../table/table.h"

REGISTER_INCOMING_PDU(PDU_Setup_HandLogRequest, CpduHandlogRequest);

CpduHandlogRequest::CpduHandlogRequest()
{}

int CpduHandlogRequest::readData(SOCKET sd)
{
    // Let superclass do the dirty work
    int rc = Cpdu::readData(sd);

    // Copy the address to an 0-terminated string
    if (rc > 0)
    {
        u_int16_t length = getAddrLen();
        if (length > 0)
        {
            char* buf = new char[length+1];
            if (buf)
            {
                char* addr = (value_ + PDU_HEADERSIZE + sizeof(pdu_handlogrequest));
                memcpy(buf, addr, length);
                buf[length] = 0;
                address_ = buf;
                delete [] buf;
            }
        }
    }

    return rc;
}

void CpduHandlogRequest::grok(SOCKET sd, CTable* table)
{
    printf("Got hand log request: %d %s\n", getNumHands(), getAddress());
    table->putHandLogRequest(sd, getAddress(), getNumHands());
}