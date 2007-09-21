#ifndef __network_pdutournamentdetailsreply_h__
#define __network_pdutournamentdetailsreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUTournamentDetailsReply
  Created  : 07/07/2000

  OVERVIEW : Lounge Tournament Details Reply PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"

class CTournamentDetailsDlg;


BEGIN_NAMESPACE(Network)


struct PDUTournamentDetailsReply : public PDUHeader
{
  // Originated from server
  CChips    buyin_;
  u_int32_t starttime_;
  u_int16_t numplayers_;
  message_t descr_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t) +
                                 sizeof(u_int32_t); }

  PDUTournamentDetailsReply(const char* buf);
  ~PDUTournamentDetailsReply();

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Tournament Details Reply PDU ---");
    PDUHeader::checkValid();
  }

private:
  void populateDialog(CTournamentDetailsDlg*);
};

END_NAMESPACE(Network)

#endif