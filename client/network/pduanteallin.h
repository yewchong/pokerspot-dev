#ifndef __network_pduanteallin_h__
#define __network_pduanteallin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUAnteAllIn
  Created  : 08/13/2000

  OVERVIEW : This pdu is used to prepare the client when
             a player goes all in in the ante.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUAnteAllIn : public PDUHeader
{
  // originated by server

  CChips bet_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(CChips::chip_amount_t); }
  PDUAnteAllIn(const char* buf)
    :
    PDUHeader(buf),
    bet_     (0)
  {
    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)&buf[PDU_HEADER_SIZE];
    bet_ = CChips_n(*pchips++).ntohl();
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Ante All-In PDU ---");
    PDUHeader::checkValid();
  }
};


END_NAMESPACE(Network)

#endif