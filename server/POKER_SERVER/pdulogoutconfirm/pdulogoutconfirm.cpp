#include "pdulogoutconfirm.h"
#include "../player/player.h"

CpduLogoutConfirm::CpduLogoutConfirm()
{
    type_ = PDU_Setup_TableLogoutConfirm;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

int CpduLogoutConfirm::SendLogoutConfirmPdu(CPlayer* player)
{
    CpduLogoutConfirm pdu;
    return pdu.sendTo(player->getConnfd());
}
