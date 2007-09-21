#ifndef __network_pducreditcardedit_h__
#define __network_pducreditcardedit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class PDUCreditCardEdit
  Created  : 04/24/2001

  OVERVIEW : Credit Card Edit PDU; sent by the lounge client
             to the server to specify credit card data.

 ****************************************************************************/

#include "network/network.h"
#include <list>


BEGIN_NAMESPACE(Network)


struct CreditCardData
{
  CreditCardData(const CString& t,
                 const CString& n,
                 const CString& m,
                 const CString& y)
    :
    cctype_(t),
    ccnumber_(n),
    ccexpmo_(m),
    ccexpyr_(y)
  {}
  CString cctype_, ccnumber_,
          ccexpmo_, ccexpyr_;
};

struct PDUCreditCardEdit : public PDUHeader
{
  // originated from client

  u_int16_t num_cards_; 

  size_t sizeOf() const;
  u_byte_t* write(char* buf) const;

  PDUCreditCardEdit(const std::list<CreditCardData>&);

private:
  std::list<CreditCardData> ccards_;
};


END_NAMESPACE(Network)

#endif