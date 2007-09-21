#ifndef __lounge_pducreditcardreply2_h__
#define __lounge_pducreditcardreply2_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardReply2
  Created  : 04/24/2001

  OVERVIEW : Server responds to client's credit card query with this
             pdu. Used to implement editing of credit cards within
             the client.

 ****************************************************************************/

#include "pdu.h"
#include <list>
#include <string>

struct CreditCardData
{
  CreditCardData(const char* cctype,
                 const char* ccnumber,
                 const char* ccexpmo,
                 const char* ccexpyr)
    :
    cctype_  (cctype),
    ccnumber_(ccnumber),
    ccexpmo_ (ccexpmo),
    ccexpyr_ (ccexpyr)
  {}

  string cctype_;
  string ccnumber_;
  string ccexpmo_;
  string ccexpyr_;
};

typedef list<CreditCardData> CreditCards;


struct pdu_ccdata
{
  u_int16_t typeIndex_;
  char digits_[16];
  char expmo_[2];
  char expyr_[2];
};


class CpduCreditCardReply2 : public Cpdu
{
public:
    CpduCreditCardReply2();

    int sendCards(SOCKET sd,
                  const list<CreditCardData>& ccards);
};


#endif
