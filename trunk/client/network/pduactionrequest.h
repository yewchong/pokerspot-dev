#ifndef __network_pduactionrequest_h__
#define __network_pduactionrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUActionRequest
  Created  : 11.11.1999

  OVERVIEW : Action Request PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUActionRequest : public PDUHeader
{
  u_int16_t player_; // slot
  CChips    value_;  // amount to pay
  CChips    raise_;  // amount to raise?
  u_int16_t msecs_;  // time allowed for thinking
  u_int16_t sequencenumber_;// id for this request

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t) +
                                 2 * sizeof(CChips::chip_amount_t); }

/*
  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(player_);
    *pint++ = htons(value_);
    *pint++ = htons(raise_);
    *pint++ = htons(msecs_);
    *pint++ = htons(sequencenumber_);
    return (u_byte_t*)pint;
  }

  PDUActionRequest(const char* username,
                   const char* password,
                   u_int16_t   player,
                   u_int16_t   amount,
                   u_int16_t   raise,
                   u_int16_t   msecs,
                   u_int16_t   sequencenumber)
    :
    PDUHeader(PDU_Play_ActionRequest,
              sizeOf(),
              username, password),
    player_  (player),
    value_   (amount),
    raise_   (raise),
    msecs_   (msecs),
    sequencenumber_(sequencenumber)
  {}
*/

  PDUActionRequest(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    player_ = ntohs(*pint++);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    value_ = CChips_n(*pchips++).ntohl();
    raise_ = CChips_n(*pchips++).ntohl();

    pint = (u_int16_t*)pchips;
    msecs_ = ntohs(*pint++);
    sequencenumber_ = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Action Request PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(player_ < 10);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   player: %d value: %f raise: %f msecs %d",
             player_, value_.asDouble(), raise_.asDouble(), msecs_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }

};


END_NAMESPACE(Network)


#endif