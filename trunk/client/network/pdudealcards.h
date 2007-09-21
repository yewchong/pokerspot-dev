#ifndef __network_pdudealcards_h__
#define __network_pdudealcards_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUDealCards
  Created  : 10.11.1999

  OVERVIEW : Deal Cards PDU.

 ****************************************************************************/

#include "network/network.h"
#include "base/suit.h"

BEGIN_NAMESPACE(Network)

USING_NS(Base);

struct PDUDealCards : public PDUHeader
{
  u_int16_t dealer_,
            card1_,
            suit1_,
            card2_,
            suit2_;
  u_int16_t msecs_;

  void execute(GameState*);

  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 6 * sizeof(u_int16_t); }

  u_byte_t* write(char* buf) const
  {    
    u_int16_t* pint = (u_int16_t*)PDUHeader::write(buf);
    *pint++ = htons(dealer_);
    *pint++ = htons(card1_);
    *pint++ = htons(suit1_);
    *pint++ = htons(card2_);
    *pint++ = htons(suit2_);
    *pint++ = htons(msecs_);
    return (u_byte_t*)pint;
  }

  PDUDealCards(const char* username,
               const char* password,
               u_int16_t   dealer,
               u_int16_t   card1,
               u_int16_t   suit1,
               u_int16_t   card2,
               u_int16_t   suit2,
               u_int16_t   msecs)
    :
    PDUHeader(PDU_Play_DealCards,
              sizeOf(),
              username, password),
    dealer_  (dealer),
    card1_   (card1),
    suit1_   (suit1),
    card2_   (card2),
    suit2_   (suit2),
    msecs_   (msecs)
  {
  }

  PDUDealCards(const char* buf)
    :
    PDUHeader(buf)
  {
    u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
    dealer_ = ntohs(*pint++);
    card1_ = ntohs(*pint++);
    suit1_ = ntohs(*pint++);
    card2_ = ntohs(*pint++);
    suit2_ = ntohs(*pint++);
    msecs_ = ntohs(*pint);
  }


  void checkValid()
  {
    PDU_DEBUG("--- Recv: Deal Cards PDU ---");
    PDUHeader::checkValid();
    PDU_ASSERT(length_ == sizeOf());
    PDU_ASSERT(dealer_ < 10);
    if (card1_ != 0 || suit1_ != 0 || card2_ != 0 || suit2_ != 0)
    {
      PDU_ASSERT(card1_ >= 2 && card1_ < 15);
      PDU_ASSERT(suit1_ > SuitBegin && suit2_ < SuitEnd);
      PDU_ASSERT(card2_ >= 2 && card2_ < 15);
      PDU_ASSERT(suit2_ > SuitBegin && suit2_ < SuitEnd);
    }
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   card2: %d suit2: %d msecs: %d", card2_, suit2_, msecs_);
    PDU_DEBUG((LPCSTR)s);
    s.Format("   dealer_: %d card1: %d suit1: %d", dealer_, card1_, suit1_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }    
};

END_NAMESPACE(Network)


#endif