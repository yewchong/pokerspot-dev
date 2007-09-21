/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBroadTableReply
  Created  : 04/24/2000

  OVERVIEW : Receive and send Broad Table Reply PDU.

             Table server sends this pdu as a reply to
             Broad Table Query pdu. Lounge server forwards
             the reply to the player who originated the query-

 ****************************************************************************/

#include "pdubroadtablereply.h"
#include "psi_assert.h"
#include "table/table.h"
#include "player/player.h"

CpduBroadTableReply::CpduBroadTableReply()
{
  type_ = PDU_Lounge_BroadTableReply;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}

int CpduBroadTableReply::sendTableReply(CTable* table,
                                        u_int16_t tableSocket,
                                        u_int16_t playerSocket)
{
  int numPlayers = table->countPlayersRaw();
  
  pdu_broadtablereply pdu;
  memset(&pdu, 0, sizeof(pdu));
  
  ASSERT(MAXBUFFERSIZE > PDU_HEADER_SIZE + sizeof(pdu_broadtablereply) + CTable::MaxPlayers * sizeof(playerinfo));
  
  pdu.avg_pot_size = table->getAveragePotSize().htonl_rep();
  pdu.flop_prct = htons(table->getFlopPercentage());
  pdu.playersocket = htons(playerSocket);
  pdu.num_players = htons(numPlayers);
  
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
  
  char* buf = value_ + PDU_HEADER_SIZE + sizeof(pdu_broadtablereply);
  playerinfo pinfo;
  memset(&pinfo, 0, sizeof(pinfo));
  
  int i = 0;
  for (i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table->getPlayerRawSlot(i);

    if ((player->getConnfd() != -1) || player->isZombie()) 
    {
      strncpy(pinfo.username, player->getUsername(), PDUSTRINGSIZE);
      strncpy(pinfo.homecity, player->getCity(), PDUSTRINGSIZE);
      pinfo.chips = player->getChips().htonl_rep();
      
      memcpy(buf, &pinfo, sizeof(pinfo));
      buf += sizeof(pinfo);
    }
  }
  
  type_ = PDU_Lounge_BroadTableReply;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_broadtablereply) +
            numPlayers * sizeof(playerinfo);
  stuffHeader();
  return sendTo(tableSocket);
};

