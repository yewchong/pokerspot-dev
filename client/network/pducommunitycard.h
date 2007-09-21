#ifndef __network_pducommunitycard_h__
#define __network_pducommunitycard_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUCommunityCard
  Created  : 10.11.1999

  OVERVIEW : Community Card PDU.

 ****************************************************************************/

#include "network/network.h"
#include "base/suit.h"

BEGIN_NAMESPACE(Network)

USING_NS(Base);

struct PDUCommunityCard : public PDUHeader
{
  u_int16_t card_,
            suit_;
  u_int16_t msecs_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  { 
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(card_);
    *pint++ = htons(suit_);
    *pint++ = htons(msecs_);
    return (u_byte_t*)pint;
  }

  PDUCommunityCard(const char* username,
                   const char* password,
                   u_int16_t   card,
                   u_int16_t   suit,
                   u_int16_t   msecs)
    :
    PDUHeader(PDU_Play_CommunityCard,
              sizeOf(),
              username, password),
    card_    (card),
    suit_    (suit),
    msecs_   (msecs)
  {
  }

  PDUCommunityCard(const char* buf)
    :
    PDUHeader(buf),
    card_ (0),
    suit_ (0),
    msecs_(0)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    card_ = ntohs(*pint++);
    suit_ = ntohs(*pint++);
    msecs_ = ntohs(*pint);
  }

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Community Card PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(card_ >= 2 && card_ < 15);
    PDU_ASSERT(suit_ > SuitBegin && suit_ < SuitEnd);

#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   card: %d suit: %d", card_, suit_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }    

};

END_NAMESPACE(Network)


#endif