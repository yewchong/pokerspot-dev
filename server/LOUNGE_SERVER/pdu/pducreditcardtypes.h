#ifndef __lounge_pducreditcardtypes_h__
#define __lounge_pducreditcardtypes_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardTypes
  Created  : 04/24/2001

  OVERVIEW : Server responds to client's credit card query with this
             pdu if the client want to know the supported credit card
             types. Used to implement editing of credit cards within
             the client.

 ****************************************************************************/

#include "pdu.h"
#include <list>
#include <string>


struct CreditCardType
{
  CreditCardType(const char* str)
    : cctype_(str)
  {}

  string cctype_;
};



class CpduCreditCardTypes : public Cpdu
{
public:
  CpduCreditCardTypes();

  int sendTypes(SOCKET sd, const list<CreditCardType>& cctypes);
};


#endif
