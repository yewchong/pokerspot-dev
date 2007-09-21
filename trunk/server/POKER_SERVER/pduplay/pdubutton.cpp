#include "pdubutton.h"

CpduButton::CpduButton(CTable *table)
{
    type_ = PDU_Play_Button;
    length_ = PDU_HEADER_SIZE + sizeof(button_pdu);
    stuffHeader();

    table_ = table;
};

int CpduButton::sendButton(void)
{
    button_pdu pdu;
    memset(&pdu, 0, sizeof(pdu));
    pdu.slot = (u_int16_t) htons(table_->getButtonSlot());

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return broadcastTo(table_->getRing());
};