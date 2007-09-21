/****************************************************************************

  Copyright (c) PokerSpot International, 1999

  Title    : class PDUCreditCardReply2
  Created  : 04/24/2001

  OVERVIEW : Extended Credit Card Reply PDU; sent by the lounge server
             to the client to specify player's credit cards for editing.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pducreditcardreply2.h"
#include "loungedlg/resource.h"
#include "loungedlg/creditcarddlg.h"


BEGIN_NAMESPACE(Network)


PDUCreditCardReply2::PDUCreditCardReply2(const char* buf)
  :
  PDUHeader (buf),
  num_cards_(0),
  pCards_   (0)
{
#ifndef PSPOT_TABLE_MODULE_
  u_int16_t* pint = (u_int16_t*)&buf[PDU_HEADER_SIZE];
  num_cards_ = ntohs(*pint++);
  u_byte_t* pbyte = (u_byte_t*)pint;

  // Rest of the data is CardInfos - extract
  // from the buffer
  size_t extraBytes = length_ - sizeOf();
  u_int16_t num_cards = extraBytes / sizeof(CardInfo);
  PDU_ASSERT(num_cards == num_cards_);
  num_cards_ = num_cards;

  if (num_cards_ > 0)
    pCards_ = new CardInfo[num_cards_];

  if (pCards_)
  { 
    memset(pCards_, 0, num_cards_ * sizeof(CardInfo));
    CardInfo* pC = pCards_;

    assert(sizeof(pCards_->card_type_) == sizeof(u_int16_t));
    assert(sizeof(pCards_->digits_) == 16);

    for (int i = 0; i < num_cards_; i++, pC++)
    {
      u_int16_t* pint = (u_int16_t*)pbyte;
      pC->card_type_ = ntohs(*pint++);
      pbyte = (u_byte_t*)pint;
      memcpy(pC->digits_, pbyte, sizeof(pC->digits_));
      pbyte += sizeof(pC->digits_);
      memcpy(pC->expmo_, pbyte, sizeof(pC->expmo_));
      pbyte += sizeof(pC->expmo_);
      memcpy(pC->expyr_, pbyte, sizeof(pC->expyr_));
      pbyte += sizeof(pC->expyr_);
    }
  }      
#endif
}

void PDUCreditCardReply2::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CCreditCardDlg::AddCreditCards(num_cards_, pCards_);
#endif
}


PDUCreditCardReply2::~PDUCreditCardReply2()
{
  delete [] pCards_;
  pCards_ = 0;
  num_cards_ = 0;
}



END_NAMESPACE(Network)

