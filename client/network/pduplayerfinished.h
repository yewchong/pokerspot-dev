#ifndef __network_pduplayerfinished_h__
#define __network_pduplayerfinished_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUTournamentPlayerFinished
  Created  : 08/22/2000

  OVERVIEW : PDU used for notifying clients that a player
             was eliminated from the tournament.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct PDUTournamentPlayerFinished : public PDUHeader
{
  // Originated from server

  string_t username_;
  u_int16_t table_;
  u_int16_t pos_;
  u_int16_t numPlayers_;

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(string_t) +
                                 3 * sizeof(u_int16_t); }
  void execute(GameState*);


  PDUTournamentPlayerFinished(const char* buf)
    :
    PDUHeader(buf),
    table_   (0),
    pos_     (0),
    numPlayers_(0)
  {
    const char* pbyte = &buf[PDU_HEADER_SIZE];
    strncpy(username_, pbyte, PDU_STRINGSIZE);
    pbyte += PDU_STRINGSIZE;

    u_int16_t* pint = (u_int16_t*)pbyte;
    table_ = ntohs(*pint++);
    pos_ = ntohs(*pint++);
    numPlayers_ = ntohs(*pint++);
  }

  ~PDUTournamentPlayerFinished()
  {}

#ifdef _DEBUG
  void checkValid()
  {
    PDU_DEBUG("--- Recv: Tournament Player Finished PDU ---");
    PDUHeader::checkValid();
  }
#endif
};

END_NAMESPACE(Network)

#endif