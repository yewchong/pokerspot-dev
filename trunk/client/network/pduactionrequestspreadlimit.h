#ifndef __network_pduactionrequestspreadlimit_h__
#define __network_pduactionrequestspreadlimit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUActionRequestSpreadLimit
  Created  : 07/02/2000

  OVERVIEW : Action Request PDU allowing spread limit raises.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUActionRequestSpreadLimit : public PDUHeader
{
  u_int16_t player_;  // slot
  CChips    value_;   // amount to pay
  CChips    raiseLo_; // raise lo limit
  CChips    raiseHi_; // raise hi limit
  u_int16_t msecs_;   // time allowed for thinking
  u_int16_t sequencenumber_;// id for this request

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t) +
                                 3 * sizeof(CChips::chip_amount_t); }

  PDUActionRequestSpreadLimit(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    player_ = ntohs(*pint++);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    value_ = CChips_n(*pchips++).ntohl();
    raiseLo_ = CChips_n(*pchips++).ntohl();
    raiseHi_ = CChips_n(*pchips++).ntohl();

    pint = (u_int16_t*)pchips;
    msecs_ = ntohs(*pint++);
    sequencenumber_ = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Spread Limit Action Request PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(player_ < 10);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   player: %d value: %f raiseLo: %f raiseHi: %f msecs %d",
             player_, value_.asDouble(), raiseLo_.asDouble(), raiseHi_.asDouble(), msecs_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }

};


END_NAMESPACE(Network)


#endif