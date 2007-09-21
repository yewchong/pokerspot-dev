#include "pduobserverlogin.h"
#include "../table/table.h"

REGISTER_INCOMING_PDU(PDU_Setup_ObserverLogin, CpduObserverLogin);

CpduObserverLogin::CpduObserverLogin()
{
    memset(sz_username, 0, sizeof(sz_username));
    memset(sz_password, 0, sizeof(sz_password));
}

void CpduObserverLogin::grok(SOCKET sd, CTable* table)
{
    // Copy username & password to zero-terminated
    // strings
	pdu_observerlogin* data = (pdu_observerlogin*)(value_ + PDU_HEADERSIZE);
    memcpy(sz_username, data->username, PDUSTRINGSIZE);
    sz_username[PDUSTRINGSIZE] = '\0';
    memcpy(sz_password, data->password, PDUSTRINGSIZE);
    sz_password[PDUSTRINGSIZE] = '\0';

    table->observerLogin(sd, this);

    // this pdu is done
    delete this;
}
