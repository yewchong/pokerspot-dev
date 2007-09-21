#ifndef __lounge_pducreditcardreply_h__
#define __lounge_pducreditcardreply_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardReply
  Created  : 04/19/2000

  OVERVIEW : Send-only credit card reply PDU.

 ****************************************************************************/


#include "pdu.h"
#include <list>

struct pdu_creditcard
{
    char last_4_digits[4];
    char card_type[PDU_STRINGSIZE];
    
    // note: no virtual functions =>
    // sizeof(pdu_creditcard) == sizeof(last_4_digits) + sizeof(card_type);

    pdu_creditcard()
    {
        memset(last_4_digits, 0, sizeof(last_4_digits));
        memset(card_type, 0, sizeof(card_type));
    }
    pdu_creditcard(const pdu_creditcard& rhs)
    {
        memcpy(last_4_digits, rhs.last_4_digits, sizeof(last_4_digits));
        memcpy(card_type, rhs.card_type, sizeof(card_type));
    }
    pdu_creditcard& operator = (const pdu_creditcard& rhs)
    {
        if (this != &rhs)
        {
            memcpy(last_4_digits, rhs.last_4_digits, sizeof(last_4_digits));
            memcpy(card_type, rhs.card_type, sizeof(card_type));
        }
        return *this;
    }
};


class CpduCreditCardReply : public Cpdu
{
public:
    CpduCreditCardReply();
    int sendCards(const list<pdu_creditcard>& cards,
                  CPlayer* player);
};


#endif