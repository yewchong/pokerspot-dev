#ifndef __network_pduplayer_h__
#define __network_pduplayer_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPlayer
  Created  : 10.11.1999

  OVERVIEW : Player PDUs.

 ****************************************************************************/

#include "network/network.h"
#include "common/chips.h"


BEGIN_NAMESPACE(Network)


struct PDUPlayer : public PDUHeader
{
  u_int16_t numplayers_; // XXX NOT USED! REMOVE
  u_int16_t slot_;
  u_int16_t state_;
  CChips    chips_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t); }
/*
  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(numplayers_);
    *pint++ = htons(slot_);
    *pint++ = htons(state_);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    *pchips++ = chips_.htonl_rep();

    pint = (u_int16_t*)pchips;
    *pint++ = htons(chips_);
    return (u_byte_t*)pint;
  }

  PDUPlayer(const char* username,
            const char* password,
            u_int16_t numplayers,
            u_int16_t slot,
            u_int16_t status,
            const CChips& chips)
    :
    PDUHeader  (PDU_Player, sizeOf(),
                username, password),
    numplayers_(numplayers),
    slot_      (slot),
    state_     (status),
    chips_     (chips)
  {}
*/

  PDUPlayer(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    numplayers_ = ntohs(*pint++);
    slot_       = ntohs(*pint++);
    state_      = ntohs(*pint++);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    chips_      = CChips_n(*pchips).ntohl();
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Player PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(slot_ < 10);
    PDU_ASSERT(state_ != 0);  
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   numplayers: %d slot: %d state: %d chips: %9.2f",
             numplayers_, slot_, state_, chips_.asDouble());
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif