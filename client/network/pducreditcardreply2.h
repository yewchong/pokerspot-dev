#ifndef __network_pducreditcardreply2_h__
#define __network_pducreditcardreply2_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUCreditCardReply2
  Created  : 04/24/2001

  OVERVIEW : Extended Credit Card Reply PDU; sent by the lounge server
             to the client to specify player's credit cards for editing.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUCreditCardReply2 : public PDUHeader
{
  // Originated by server

  u_int16_t num_cards_;

  struct CardInfo
  {
    u_int16_t card_type_; // type index
    char digits_[16];
    char expmo_[2];
    char expyr_[2];
  };
  CardInfo* pCards_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUCreditCardReply2(const char* buf);
  ~PDUCreditCardReply2();

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Credit Card Reply2 PDU ---");
    PDUHeader::checkValid();
#ifdef PDU_DEBUG_
    CString s;
    s.Format("   numcards: %d", num_cards_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif