#include "pduobserverlogout.h"
#include "../table/table.h"

REGISTER_INCOMING_PDU(PDU_Setup_ObserverLogout, CpduObserverLogout);

CpduObserverLogout::CpduObserverLogout()
{}

void CpduObserverLogout::grok(SOCKET sd, CTable* table)
{
    table->observerLogout(sd, this);

    // This pdu is done
    delete this;
}



