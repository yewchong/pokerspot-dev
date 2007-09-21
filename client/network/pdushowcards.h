#ifndef __network_pdushowcards_h__
#define __network_pdushowcards_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUShowCards
  Created  : 01/20/2000

  OVERVIEW : Show Cards PDU; reply to Show Cards Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUShowCards : public PDUHeader
{
  // originated by client

  u_int16_t action_;

  enum ShowAction
  {
    Hide = 0,
    Show = 1
  };

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  virtual u_byte_t* write(char* buffer) const
  {
#ifdef PCLIENT_MODULE_
    PDU_DEBUG("--- Send: Show Cards PDU ---");
#endif
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buffer);
    *pint++ = htons(action_);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   action: %d ", action_);
    PDU_DEBUG(s);
#endif
    return (u_byte_t*)pint;
  }

  PDUShowCards(const char* username,
               const char* password,
               u_int16_t   action)
    :
    PDUHeader(PDU_Play_ShowCards,
              sizeOf(),
              username, password),
    action_  (action)
  {}

  PDUShowCards(const char* buf)
    :
    PDUHeader(buf),
    action_(0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    action_ = ntohs(*pint++);
  }
};


END_NAMESPACE(Network)

#endif