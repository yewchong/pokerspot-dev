#ifndef __network_pduaccountbalance_h__
#define __network_pduaccountbalance_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUAccountBalance
  Created  : 02/20/2000

  OVERVIEW : Account balance PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"
#include "common/stringoutput.h"


BEGIN_NAMESPACE(Network)


struct PDUAccountBalance : public PDUHeader
{
  // originated by server

  CChips balance_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(CChips::chip_amount_t); }

  PDUAccountBalance(const char* buf)
    :
    PDUHeader(buf),
    balance_ (0)
  {
    CChips::chip_amount_t* pint = (CChips::chip_amount_t*)&buf[PDU_HEADER_SIZE];
    balance_ = CChips_n(*pint++).ntohl();
  }

  void checkValid()
  {
    OutputDebugString("--- Recv: Account Balance PDU ---\n");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CStrOut s;
    s << "   balance: " << balance_ << '\n';
    OutputDebugString(s.str());
#endif
  }
};


END_NAMESPACE(Network)

#endif