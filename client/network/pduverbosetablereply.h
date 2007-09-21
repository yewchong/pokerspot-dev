#ifndef __network_pduverbosetablereply_h__
#define __network_pduverbosetablereply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUVerboseTableReply
  Created  : 02/25/2000

  OVERVIEW : Lounge Verbose Table Reply PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"

class CTableDetailsDlg;


BEGIN_NAMESPACE(Network)


struct PDUVerboseTableReply : public PDUHeader
{
  // Originated from server
  u_int16_t unused_;
  CChips avg_pot_size_;
  u_int16_t flop_prct_;
  u_int16_t num_players_;

#pragma NETWORK_ALIGN_BEGIN
  struct PlayerInfo
  {
    char username_[PDU_STRINGSIZE];
    char homecity_[PDU_STRINGSIZE];
    CChips::chip_amount_t chips_;
  };
#pragma NETWORK_ALIGN_END

  PlayerInfo* pPlayers_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t); } 

  PDUVerboseTableReply(const char* buf);
  ~PDUVerboseTableReply();

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Verbose Table Reply PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   avg pot: %9.2f flop: %d players: %d",
             avg_pot_size_.asDouble(), flop_prct_, num_players_);
    PDU_DEBUG(s);
#endif
  }

private:
  void populateDialog(CTableDetailsDlg*);
};

END_NAMESPACE(Network)

#endif