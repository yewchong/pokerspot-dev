#ifndef __network_pduactionecho_h__
#define __network_pduactionecho_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUActionEcho
  Created  : 28.12.1999

  OVERVIEW : Action Echo PDU.

 ****************************************************************************/

#include "network/network.h"
#include "network/pduaction.h"
#include "common/chips.h"

BEGIN_NAMESPACE(Network)


struct PDUActionEcho : public PDUHeader
{
  // Originated from server
  u_int16_t slot_;
  u_int16_t action_;
  CChips amount_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t) +
                                 sizeof(CChips::chip_amount_t); }

/*
  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Action Echo PDU ---");
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(slot_);
    *pint++ = htons(action_);
    *pint++ = htons(amount_);
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   slot: %d action: %d amount: %d", slot_, action_, amount_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }

  PDUActionEcho(const char* username,
                const char* password,
                u_int16_t   slot,
                u_int16_t   action,
                u_int16_t   amount)
    :
    PDUHeader(PDU_Play_ActionEcho,
              sizeOf(),
              username, password),
    slot_  (slot),
    action_(action),
    amount_(amount)
  {}
*/

  PDUActionEcho(const char* buf)
    :
    PDUHeader(buf),
    slot_    (0),
    action_  (0),
    amount_  (0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    slot_ = ntohs(*pint++);
    action_ = ntohs(*pint++);

    CChips::chip_amount_t* pchips = (CChips::chip_amount_t*)pint;
    amount_ = CChips_n(*pchips++).ntohl();
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Action Echo PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(action_ == PDUAction::Call ||
               action_ == PDUAction::Fold ||
               action_ == PDUAction::Raise ||
               action_ == PDUAction::Ante ||
               action_ == PDUAction::SitOut);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   slot: %d action: %d amount: %9.2f",
             slot_, action_, amount_.asDouble());
    PDU_DEBUG((LPCSTR)s);
#endif
  }

};


END_NAMESPACE(Network)


#endif