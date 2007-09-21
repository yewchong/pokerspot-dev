#ifndef __network_pduallin_h__
#define __network_pduallin_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUAllIn
  Created  : 02/11/2000

  OVERVIEW : All-In PDU; sent by the server when a player
             goes all-in.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUAllIn : public PDUHeader
{
  // originated by server

  u_int16_t slot_;
  CChips to_pay_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t); }
/*
  PDUAllIn(const char* username,
           const char* password,
           u_int16_t   slot,
           u_int16_t   to_pay)
    :
    PDUHeader(PDU_Play_AllIn, sizeOf(),
              username, password),
    slot_    (slot),
    to_pay_  (to_pay)
  {}
*/

  PDUAllIn(const char* buf)
    :
    PDUHeader(buf),
    slot_    (0),
    to_pay_  (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    slot_ = ntohs(*pint++);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    to_pay_ = CChips_n(*pchips++).ntohl();
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: All-In PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   slot: %d, to pay: %9.2f", slot_, to_pay_.asDouble());
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif