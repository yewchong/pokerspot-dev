/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduPlayer
  Created  : 1/1/2001

  OVERVIEW : This class is used to send player state to clients.

 ****************************************************************************/

#include "pduplayer.h"
#include "player/player.h"
#include "table/table.h"

CpduPlayer::CpduPlayer(CTable* table)
  :
  table_(table)
{}

int CpduPlayer::SendPlayerPdu(CPlayer* player, u_int16_t reason)
{
  CpduPlayer pdu(CTable::Instance());
  return pdu.sendPlayerPdu(player, reason);
}

int CpduPlayer::sendPlayerPdu(CPlayer* player, u_int16_t reason)
{
  type_ = PDU_Player;
  length_ = PDU_HEADER_SIZE + sizeof(player_pdu);
  stuffHeader();

//  printf("%d\n", length_);
//  ASSERT(length_ == 14);

  player_pdu data;
  memset(&data, 0, sizeof(data));
  data.numplayers = htons(0); // not used
  data.slot = htons(player->getSlot());
  data.state = htons(reason);
  data.chips = player->getChips().htonl_rep();

  memcpy(value_ + PDU_HEADER_SIZE, &data, sizeof(data));

  return broadcastTo(table_->getRing());
}

int CpduPlayer::sendPlayerRaw(CPlayer* player, u_int16_t reason)
{
  type_ = PDU_Player;
  length_ = PDU_HEADER_SIZE + sizeof(player_pdu);
  stuffHeader();

//  printf("%d\n", length_);
//  ASSERT(length_ == 14);

  player_pdu data;
  memset(&data, 0, sizeof(data));
  data.numplayers = htons(0); // not used
  data.slot = htons(player->getSlot());
  data.state = htons(reason);
  data.chips = player->getChips().htonl_rep();

  memcpy(value_ + PDU_HEADER_SIZE, &data, sizeof(data));

  int i = 0;
  for (i = 0; i < CTable::MaxPlayers; i++)
  {
    CPlayer* player = table_->getPlayerRawSlot(i);
    if (player->getConnfd() != -1)
    {
      sendTo(player->getConnfd());
    }
  }   

  for (i = 0; i < CTable::MaxObservers; i++)
  {
    CPlayer* player = table_->getObserver(i);
    if (player && (player->getConnfd() != -1))
    {
      sendTo(player->getConnfd());
    }
  }   

  return 0;
}

int CpduPlayer::sendChipUpdate(CPlayer* newPlayer, CPlayer* oldPlayer)
{
  type_ = PDU_Player;
  length_ = PDU_HEADER_SIZE + sizeof(player_pdu);
  stuffHeader();

//  printf("%d\n", length_);
//  ASSERT(length_ == 14);
  
  player_pdu data;
  memset(&data, 0, sizeof(data));
  data.numplayers = htons(0); // not used
  data.slot = htons(oldPlayer->getSlot());
  data.state = htons(PLAYER_CHIPUPDATE);
  data.chips = oldPlayer->getChips().htonl_rep();
  
  memcpy(value_ + PDU_HEADER_SIZE, &data, sizeof(data));
  
  return sendTo(newPlayer->getConnfd());
}
