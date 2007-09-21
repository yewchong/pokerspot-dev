#ifndef __network_pdupocketcard_h__
#define __network_pdupocketcard_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUPocketCard
  Created  : 12.12.1999

  OVERVIEW : Pocket Card PDU.

 ****************************************************************************/

#include "network/network.h"

BEGIN_NAMESPACE(Network)


struct PDUPocketCard : public PDUHeader
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

  PDUPocketCard(const char* username,
                const char* password,
                u_int16_t   card,
                u_int16_t   suit,
                u_int16_t   msecs)
    :
    PDUHeader(PDU_Play_PocketCard,
              sizeOf(),
              username, password),
    card_    (card),
    suit_    (suit),
    msecs_   (msecs)
  {
  }

  PDUPocketCard(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    card_ = ntohs(*pint++);
    suit_ = ntohs(*pint++);
    msecs_ = ntohs(*pint++);
  }


  void checkValid()
  {
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(card_ >= 2 && card_ < 15);
    PDU_ASSERT(suit_ < 4);
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   card: %d suit: %d msecs: %d", card_, suit_, msecs_);
    PDU_DEBUG((LPCSTR)s);
#endif
    PDUHeader::checkValid();
    PDU_DEBUG("--- Recv: Pocket Card PDU ---");
  }    
};


END_NAMESPACE(Network)


#endif