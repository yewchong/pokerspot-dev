#include "pdulogin.h"
#include "../table/table.h"

REGISTER_INCOMING_PDU(PDU_Setup_TableLogin, CpduLogin);

CpduLogin::CpduLogin()
{
    memset(sz_username, 0, sizeof(sz_username));
    memset(sz_password, 0, sizeof(sz_password));
}

CpduLogin::CpduLogin(Cpdu *pCpdu)
{
    memset(sz_username, 0, sizeof(sz_username));
    memset(sz_password, 0, sizeof(sz_password));

	type_ = pCpdu->type_;
	memcpy(value_, pCpdu->value_, MAXBUFFERSIZE);
	length_ = pCpdu->length_;
};


void CpduLogin::grok(SOCKET sd, CTable* table)
{
    // Copy username & password to zero-terminated
    // strings
	pdu_login_t* data = (pdu_login_t*)(value_ + PDU_HEADERSIZE);
    memcpy(sz_username, data->username, PDUSTRINGSIZE);
    sz_username[PDUSTRINGSIZE] = '\0';
    memcpy(sz_password, data->password, PDUSTRINGSIZE);
    sz_password[PDUSTRINGSIZE] = '\0';

    table->tableLogin(sd, this);

    // this pdu is done
    delete this;
}
