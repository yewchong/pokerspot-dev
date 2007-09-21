/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUCreditCardReply
  Created  : 03/13/2000

  OVERVIEW : Credit Card Reply PDU. The server sends this
             PDU as a reply to the Credit Card Query PDU.

 ****************************************************************************/

#include "stdafx.h"
#include "network/pducreditcardresponse.h"
#ifndef PSPOT_TABLE_MODULE_
#include "loungedlg/cashierdlg.h"
#endif


BEGIN_NAMESPACE(Network)


PDUCreditCardReply::PDUCreditCardReply(const char* buf)
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

    assert(sizeof(pCards_->last_4_digits_) == 4);
    assert(sizeof(pCards_->card_type_) == PDU_STRINGSIZE);

    for (int i = 0; i < num_cards_; i++, pC++)
    {
      memcpy(pC->last_4_digits_, pbyte, sizeof(pC->last_4_digits_));
      pbyte += sizeof(pC->last_4_digits_);
      memcpy(pC->card_type_, pbyte, sizeof(pC->card_type_));
      pbyte += sizeof(pC->card_type_);
    }
  }      
#endif
}


void PDUCreditCardReply::execute(GameState*)
{
#ifndef PSPOT_TABLE_MODULE_
  CCashierDlg::AddCreditCards(num_cards_, pCards_);
#endif
}


PDUCreditCardReply::~PDUCreditCardReply()
{
  delete [] pCards_;
  pCards_ = 0;
  num_cards_ = 0;
}



END_NAMESPACE(Network)

