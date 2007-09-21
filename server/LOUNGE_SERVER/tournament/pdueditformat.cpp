/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduEditFormat
  Created  : 08/25/2000

  OVERVIEW : 

 ****************************************************************************/

/* NOT IMPLEMENTED

#include "pdueditformat.h"
#include "pdueditrules.h"
#include "lounge/player.h"

REGISTER_INCOMING_PDU(PDU_Tournament_EditFormat, CpduEditFormat);

CpduEditFormat::CpduEditFormat()
{}

void CpduEditFormat::grok(SOCKET, CPlayer* player)
{
    CpduEditRules pdu;
    pdu.sendFormat(player->getSocket());
}

*/