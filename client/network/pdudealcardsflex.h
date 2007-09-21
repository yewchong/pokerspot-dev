#ifndef __network_pdudealcardsflex_h__
#define __network_pdudealcardsflex_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUDealCardsFlex
  Created  : 04/11/2000

  OVERVIEW : Flexible Deal Cards PDU.

 ****************************************************************************/

#include "network/network.h"


BEGIN_NAMESPACE(Network)

struct CardEntry
{
  u_int16_t suit_;
  u_int16_t value_;
};

struct PDUDealCardsFlex : public PDUHeader
{
  // Originated by server

  enum CardType
  {
    CT_Hole      = 1,
    CT_Community = 2,
    CT_Indian    = 3,
    CT_Dummy     = 4
  };

  u_int16_t player_slot_;
  u_int16_t card_type_;
  u_int16_t num_cards_;

  u_int16_t msecs_; // not pdu data!

  CardEntry* pCards_;

  void execute(GameState*);
  size_t sizeOf() const { return PDUHeader::sizeOf() +
                                 3 * sizeof(u_int16_t); }

  PDUDealCardsFlex(const char* buf);
  PDUDealCardsFlex()
    :
    PDUHeader (PDU_Play_DealCardsFlex, sizeOf(), NULL, NULL),
    player_slot_(0),
    card_type_  (0),
    num_cards_  (0),
    msecs_      (100),
    pCards_     (0)
  {}
  ~PDUDealCardsFlex();

  void checkValid()
  {
    PDU_DEBUG("--- Recv: Flexible Deal Cards PDU ---");
    PDUHeader::checkValid();
#ifdef PCLIENT_MODULE_
    CString s;
    s.Format("   card type: %d, cards: %d", card_type_, num_cards_);
    PDU_DEBUG((LPCSTR)s);
#endif
  }
};


END_NAMESPACE(Network)

#endif