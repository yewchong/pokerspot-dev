#include "pdulogout.h"
#include "../table/table.h"
#include "../poller/poller.h"

REGISTER_INCOMING_PDU(PDU_Setup_TableLogout, CpduLogout);

CpduLogout::CpduLogout()
{}

void CpduLogout::grok(SOCKET sd, CTable* table)
{
    table->tableLogout(sd, this);

    // This pdu is done
    delete this;
}



