#ifndef __network_pduaction_h__
#define __network_pduaction_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUAction
  Created  : 11.11.1999

  OVERVIEW : Action PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUAction : public PDUHeader
{
  u_int16_t action_;
  u_int16_t sequencenumber_;

  enum Action
  {
    Fold  = 1,
    Call  = 2,
    Raise = 3,
    Ante  = 4,  // when posting blind
    SitOut = 5,  // when posting blind
    SitIn = 6,
    AnteAllIn = 7
  };

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 2 * sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    PDU_DEBUG("--- Send: Action PDU ---");
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(action_);
    *pint++ = htons(sequencenumber_);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   action: %d ", action_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }

  PDUAction(const char* username,
            const char* password,
            u_int16_t   action,
            u_int16_t   sequencenumber)
    :
    PDUHeader(PDU_Play_Action,
              sizeOf(),
              username, password),
    action_  (action),
    sequencenumber_(sequencenumber)
  {}

  PDUAction(const char* buf)
    :
    PDUHeader(buf),
    action_  (0),
    sequencenumber_(0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    action_ = ntohs(*pint++);
    sequencenumber_ = ntohs(*pint++);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Action PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(action_ == Call ||
               action_ == Fold ||
               action_ == Raise ||
               action_ == SitOut);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   action: %d", action_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }

};


END_NAMESPACE(Network)

#endif