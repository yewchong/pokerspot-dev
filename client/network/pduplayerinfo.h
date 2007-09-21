#ifndef __network_pduplayerinfo_h__
#define __network_pduplayerinfo_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPlayerInfo
  Created  : 03/01/2000

  OVERVIEW : Player Info PDU. This PDU is sent by the
             server at login to inform the client about
             players sitting at the table.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUPlayerInfo : public PDUHeader
{
  // Originated by server

  u_int16_t num_players_;

#pragma pack (push, 2)
  struct PlayerInfo
  {
    u_int16_t slot_;
    CChips::chip_amount_t chips_;
    u_int16_t state_;
    char username_[PDU_STRINGSIZE];
    char city_[PDU_STRINGSIZE];
  };
#pragma pack (pop)

  PlayerInfo* pPlayers_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUPlayerInfo(const char* buf);
  PDUPlayerInfo()
    :
    PDUHeader(PDU_Setup_PlayerInfo, sizeOf(), NULL, NULL),
    num_players_(0),
    pPlayers_   (0)
  {}
  ~PDUPlayerInfo();

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Player Info PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   numplayers: %d", num_players_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif