/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSaveRules
  Created  : 08/29/2000

  OVERVIEW : 

 ****************************************************************************/

#include "pdusaverules.h"
#include "lounge/player.h"
#include "pdu/pdunotify.h"
#include "dbhandler/dbinterface.h"
#include "sys.h"

REGISTER_INCOMING_PDU(PDU_Tournament_SaveRules, CpduSaveRules);


CpduSaveRules::CpduSaveRules()
{}

void CpduSaveRules::grok(SOCKET, CPlayer* player)
{
	u_int16_t flags = ntohs(*(value_ + PDU_HEADERSIZE));
    const char* data = (value_ + PDU_HEADERSIZE + sizeof(u_int16_t));

    int length = length_ - (PDU_HEADERSIZE + sizeof(u_int16_t));

    if (length > 0)
    {
        FILE* f = fopen("tournament.tdl", "wt");
        if (f)
        {
            fwrite(data, 1, length, f);
            fclose(f);
            f = NULL;
        }
    }
}

