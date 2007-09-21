/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayerInfo
  Created  : 1/1/2001

  OVERVIEW : This class is used by the table server to send
             - current state of players to a new player
             - state of new player to players currently at the table

 ****************************************************************************/

#include "pduplayerinfo.h"
#include "ring/ring.h"
#include "table/table.h"
#include "player/player.h"


CpduPlayerInfo::CpduPlayerInfo(CTable* table)
  : 
  table_(table)
{}

int CpduPlayerInfo::sendPlayerInfo(CPlayer* newplayer)
{
  int rc = 0;

  char* buf = value_ + PDU_HEADER_SIZE;

  //
  // Inform new player about existing players
  //
  pdu_playerinfo_t playerInfo;
  player_entry_t playerEntry;

  int numPlayers = table_->countPlayersRaw();
  playerInfo.num_players = htons(numPlayers);
  memcpy(buf, &playerInfo, sizeof(playerInfo));
  buf += sizeof(playerInfo);

  type_ = PDU_Setup_PlayerInfo;
  length_ = PDU_HEADER_SIZE + sizeof(playerInfo) + numPlayers * sizeof(playerEntry);
  stuffHeader();

  int i = 0;
  for (i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table_->getPlayerRawSlot(i);

    if ((player->getConnfd() != -1) || player->isZombie())
    {
      playerEntry.slot = htons(player->getSlot());
      playerEntry.state = htons(player->getState());
      playerEntry.chips = player->getChips().htonl_rep();
      strncpy(playerEntry.username, player->getUsername(), PDUSTRINGSIZE);
      strncpy(playerEntry.city, player->getCity(), PDUSTRINGSIZE);
      
      memcpy(buf, &playerEntry, sizeof(playerEntry));
      buf += sizeof(playerEntry);
    }
  }

  if (newplayer->getConnfd() != -1)
    rc = sendTo(newplayer->getConnfd());

  //
  // Inform existing players about new player
  //
  buf = value_ + PDU_HEADER_SIZE;
  length_ = PDU_HEADER_SIZE + sizeof(playerInfo) + 1 * sizeof(playerEntry);
  stuffHeader();

  numPlayers = 1;
  playerInfo.num_players = htons(numPlayers);
  memcpy(buf, &playerInfo, sizeof(playerInfo));
  buf += sizeof(playerInfo);

  playerEntry.slot = htons(newplayer->getSlot());
  playerEntry.state = htons(newplayer->getState());
  playerEntry.chips = newplayer->getChips().htonl_rep();
  strncpy(playerEntry.username, newplayer->getUsername(), PDUSTRINGSIZE);
  strncpy(playerEntry.city, newplayer->getCity(), PDUSTRINGSIZE);

  memcpy(buf, &playerEntry, sizeof(playerEntry));

  for (i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table_->getPlayerRawSlot(i);
    if ((player->getConnfd() != -1) && player != newplayer)
    {
      sendTo(player->getConnfd());
    }
  }

  // Send observers too
  for (i = 0; i < CTable::MaxObservers; i++)
  {
    CPlayer* player = table_->getObserver(i);
    if (player && (player->getConnfd() != -1))
    {
      sendTo(player->getConnfd());
    }
  }   
  
  
  return rc;
};

int CpduPlayerInfo::sendObserverInfo(CPlayer* newplayer)
{
  int rc = 0;
  
  char* buf = value_ + PDU_HEADER_SIZE;
  
  //
  // Inform new player about existing players
  //
  pdu_playerinfo_t playerInfo;
  player_entry_t playerEntry;
  
  int numPlayers = table_->countPlayersRaw();
  playerInfo.num_players = htons(numPlayers);
  memcpy(buf, &playerInfo, sizeof(playerInfo));
  buf += sizeof(playerInfo);
  
  type_ = PDU_Setup_PlayerInfo;
  length_ = PDU_HEADER_SIZE + sizeof(playerInfo) + numPlayers * sizeof(playerEntry);
  stuffHeader();
  
  int i = 0;
  for (i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table_->getPlayerRawSlot(i);
    
    if ((player->getConnfd() != -1) || player->isZombie())
    {
      playerEntry.slot = htons(player->getSlot());
      playerEntry.state = htons(player->getState());
      playerEntry.chips = player->getChips().htonl_rep();
      strncpy(playerEntry.username, player->getUsername(), PDUSTRINGSIZE);
      strncpy(playerEntry.city, player->getCity(), PDUSTRINGSIZE);
      
      memcpy(buf, &playerEntry, sizeof(playerEntry));
      buf += sizeof(playerEntry);
    }
  }
  
  return sendTo(newplayer->getConnfd());
};