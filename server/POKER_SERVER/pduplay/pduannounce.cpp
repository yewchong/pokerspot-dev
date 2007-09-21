/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduAnnounce
  Created  : 08/13/2000

  OVERVIEW : This pdu is used to announce a winner of a hand.

 ****************************************************************************/

#include "pduannounce.h"
#include "player/player.h"
#include "table/table.h"
#include "common/stringoutput.h"

struct announce_pdu
{
  u_int16_t slot;
  char announcement[MAXANNOUNCE];
};

CpduAnnounce::CpduAnnounce(CTable* table)
  :
  table_(table)
{
  type_ = PDU_Play_Announce;
  length_ = 0;
}

int CpduAnnounce::sendAnnounce(CPlayer* player, const CChips& chips)
{
  announce_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));
  pdu.slot = htons(player->getSlot());

  CStrOut s;
  s << player->getUsername() << " won " << chips << " chips!";
  strncpy(pdu.announcement, s.str(), MAXANNOUNCE);
  length_ = PDU_HEADER_SIZE + sizeof(pdu.slot) + strlen(pdu.announcement) + 1;
  stuffHeader();

  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu.slot) + strlen(pdu.announcement) + 1);

  return broadcastTo(table_->getRing());
}

int CpduAnnounce::sendAnnounce(CPlayer* player,
                               const CChips& chips,
                               const char* handType,
                               const char* handInfo)
{
  announce_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));
  pdu.slot = htons(player->getSlot());
  
  CStrOut s;
  if (handInfo != NULL)
  {
    s << player->getUsername() << " won " << chips
      << " chips with a " << handType << '('
      << handInfo << ")!";
    strncpy(pdu.announcement, s.str(), MAXANNOUNCE);

    //sprintf(pdu.announcement, "%s won %9.2f chips with a %s (%s)!",
    //        player->getUsername(), 
    //        chips.asDouble(), handType, handInfo);
  }
  else
  {
    s << player->getUsername() << " won " << chips
      << " chips with a " << handType << '!';
    strncpy(pdu.announcement, s.str(), MAXANNOUNCE);

    //sprintf(pdu.announcement, "%s won %9.2f chips with a %s!",
    //        player->getUsername(), chips.asDouble(), handType);
  }
  length_ = PDU_HEADER_SIZE + sizeof(pdu.slot) + strlen(pdu.announcement) + 1;
  stuffHeader();
  
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu.slot) + strlen(pdu.announcement) + 1);
  
  return broadcastTo(table_->getRing());
}

int CpduAnnounce::sendAnnounce(const char* msg)
{
  announce_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.slot = htons(0xffff);
  strncpy(pdu.announcement, msg, 255);
  length_ = PDU_HEADER_SIZE + sizeof(pdu.slot) + strlen(pdu.announcement) + 1;
  stuffHeader();
  
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu.slot) + strlen(pdu.announcement) + 1);
  
  return broadcastTo(table_->getRing());
}

// 7-stud
int CpduAnnounce::sendAnnounce(const char* msg, int flags)
{
  announce_pdu pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.slot = htons(flags);
  strncpy(pdu.announcement, msg, 255);
  length_ = PDU_HEADER_SIZE + sizeof(pdu.slot) + strlen(pdu.announcement) + 1;
  stuffHeader();
  
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu.slot) + strlen(pdu.announcement) + 1);
  
  return broadcastTo(table_->getRing());
}