/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSaveFormat
  Created  : 08/29/2000

  OVERVIEW : 

 ****************************************************************************/

#include "pdusaveformat.h"
#include "lounge/player.h"
#include "dbhandler/dbinterface.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Tournament_SaveFormat, CpduSaveFormat);


CpduSaveFormat::CpduSaveFormat()
{}

void CpduSaveFormat::grok(SOCKET, CPlayer* player)
{
	u_int16_t flags = ntohs(*(value_ + PDU_HEADERSIZE));
    const char* data = (value_ + PDU_HEADERSIZE + sizeof(u_int16_t));

    int length = length_ - (PDU_HEADERSIZE + sizeof(u_int16_t));

    if (length > 0)
    {
        CdbInterface::Inst()->setTournamentFormat(data);
    }
}

