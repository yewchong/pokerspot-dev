#ifndef __network_pdublindrequest_h__
#define __network_pdublindrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUBlindRequest
  Created  : 12.11.1999

  OVERVIEW : Blind Request PDU.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUBlindRequest : public PDUHeader
{
  u_int16_t player_;          // player slot
  CChips    value_;           // amount to post
  u_int16_t msecs_;           // time allowed for thinking
  u_int16_t sequencenumber_;  // id for this event

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t); }
/*
  u_byte_t* write(char* buf) const
  {
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(player_);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    *pchips++ = value_.htonl_rep();

    pint = (u_int16_t*)pchip;
    *pint++ = htons(msecs_);
    *pint++ = htons(sequencenumber_);
    return (u_byte_t*)pint;
  }

  PDUBlindRequest(const char* username,
                  const char* password,
                  u_int16_t player,
                  const CChips& value,
                  u_int16_t msecs,
                  u_int16_t sequencenumber)
    :
    PDUHeader(PDU_Play_BlindRequest,
              sizeOf(),
              username, password),
    player_  (player),
    value_   (value),
    msecs_   (msecs),
    sequencenumber_(sequencenumber)
  {}
*/

  PDUBlindRequest(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    player_ = ntohs(*pint++);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    value_ = CChips_n(*pchips++).ntohl();

    pint = (u_int16_t*)pchips;
    msecs_ = ntohs(*pint++);
    sequencenumber_ = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Blind Post Request PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(player_ < 10);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   player: %d value: %9.2f msecs: %d",
             player_, value_.asDouble(), msecs_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif