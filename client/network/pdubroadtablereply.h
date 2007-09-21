#ifndef __network_pdubroadtablereply_h__
#define __network_pdubroadtablereply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : struct PDUBroadTableReply
  Created  : 02/25/2000

  OVERVIEW : Lounge Broad Table Reply PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUBroadTableReply : public PDUHeader
{
  // Originated from server
  u_int16_t gametype_;
  CChips    lolimit_;
  CChips    hilimit_;
  u_int16_t tablenum_;

  PDUBroadTableReply(const char* buf)
    :
    PDUHeader(buf)
  {}

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Lounge Broad Table Reply PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
#endif
  }
};


#endif