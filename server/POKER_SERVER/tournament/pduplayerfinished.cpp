/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduPlayerFinished
  Created  : 08/22/2000

  OVERVIEW : This pdu is used to notify clients that a player was eliminated
             from tournament.

 ****************************************************************************/

#include "tournament/pduplayerfinished.h"
#include "table/table.h"
#include "player/player.h"

struct pdu_playerfinished
{
  char username[PDU_STRINGSIZE];
  u_int16_t table;
  u_int16_t pos;
  u_int16_t players;
};

CpduPlayerFinished::CpduPlayerFinished()
{
    type_ = PDU_Tournament_PlayerFinished;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_playerfinished);
    stuffHeader();
}

int CpduPlayerFinished::sendPlayerFinished(CTable*   table,
                                           CPlayer*  player,
                                           u_int16_t tableNum,
                                           u_int16_t pos,
                                           u_int16_t players)
{
    pdu_playerfinished pdu;
    memset(&pdu, 0, sizeof(pdu));
    strncpy(pdu.username, player->getUsername(), PDU_STRINGSIZE);
    pdu.table = htons(tableNum);
    pdu.pos = htons(pos);
    pdu.players = htons(players);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    int i = 0;
    // Send directly to players because this function is
    // called from within rebuildring
    for (i = 0; i < CTable::MaxPlayers; i++)
    {
        CPlayer* p = table->getPlayerRawSlot(i);
        if (p && (p->getConnfd() != -1))
        {
            if (sendTo(p->getConnfd()) == -1)
                p->setSendError(true);
        }
    }

    // Send to observers too
    for (i = 0; i < CTable::MaxObservers; i++)
    {
        CPlayer* observer = table->getObserver(i);
        if (observer && (observer->getConnfd() != -1))
        {
            if (sendTo(observer->getConnfd()) == -1)
                observer->setSendError(true);
        }
    }

    return 0;
}
