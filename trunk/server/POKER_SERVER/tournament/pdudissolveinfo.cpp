/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduDissolveInfo
  Created  : 07/10/2000

  OVERVIEW : Table sends dissolve information to lounge server using
             this PDU.

 ****************************************************************************/

#include "pdudissolveinfo.h"
#include "table/table.h"
#include "player/player.h"


REGISTER_INCOMING_PDU(PDU_Tournament_DissolveInfo, CpduDissolveInfo);


CpduDissolveInfo::CpduDissolveInfo()
{
  type_ = PDU_Tournament_DissolveInfo;
  length_ = PDU_HEADER_SIZE;
  stuffHeader();
}

int CpduDissolveInfo::sendDissolveInfo(SOCKET sd, CTable* table)
{
  int numPlayers = table->countPlayersRaw();

  int numPlayers2 = 0;
  int i = 0;
  for (i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table->getPlayerRawSlot(i);
    if (!player->matchState(PLAYER_STATE_UNUSED))
    {
      numPlayers2++;
    }
  }
  
  if (numPlayers != numPlayers2)
  { // debugging:
    char buf[100];
    sprintf(buf, "TOURNAMENT: error: np: %i np2: %i\n", numPlayers, numPlayers2);
    Sys_LogError(buf);
    numPlayers = numPlayers2;
  }
  
  pdu_dissolveinfo pdu;
  memset(&pdu, 0, sizeof(pdu));
  
  ASSERT(MAXBUFFERSIZE > PDU_HEADER_SIZE + sizeof(pdu_dissolveinfo) + CTable::MaxPlayers * sizeof(dissolve_playerinfo));
  
  pdu.num_players = htons(numPlayers);
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
  
  char* buf = value_ + PDU_HEADER_SIZE + sizeof(pdu);
  dissolve_playerinfo pinfo;
  memset(&pinfo, 0, sizeof(pinfo));
  
  for (i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table->getPlayerRawSlot(i);
    if (!player->matchState(PLAYER_STATE_UNUSED))
    {
      strncpy(pinfo.username, player->getUsername(), PDUSTRINGSIZE);
      pinfo.chips = player->getChips().htonl_rep();
      
      memcpy(buf, &pinfo, sizeof(pinfo));
      buf += sizeof(pinfo);
    }
  }
  
  length_ = PDU_HEADER_SIZE + sizeof(pdu_dissolveinfo) +
            numPlayers * sizeof(dissolve_playerinfo);
  stuffHeader();
  
  return sendTo(sd);
}