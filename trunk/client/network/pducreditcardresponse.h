#ifndef __network_pducreditcardresponse_h__
#define __network_pducreditcardresponse_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUCreditCardReply
  Created  : 03/13/2000

  OVERVIEW : Credit Card Reply PDU. The server sends this
             PDU as a response to the Credit Card Query PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)


struct PDUCreditCardReply : public PDUHeader
{
  // Originated by server

  u_int16_t num_cards_;

  struct CardInfo
  {
    char last_4_digits_[4];
    char card_type_[PDU_STRINGSIZE];
  };
  CardInfo* pCards_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 sizeof(u_int16_t); }

  PDUCreditCardReply(const char* buf);
  ~PDUCreditCardReply();

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Credit Card Reply PDU ---");
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