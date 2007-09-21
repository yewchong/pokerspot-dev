/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardEdit
  Created  : 04/24/2001

  OVERVIEW : Client specifies the player's credit cards to the
             server with this pdu. Used to implement editing of
             credit cards within the client.

 ****************************************************************************/

#include "pdu.h"

#include "pdu/pducreditcardedit.h"
#include "pdu/pducreditcardreply2.h"
#include "lounge/player.h"
#include "dbhandler/dbinterface.h"

extern string GetCCTypeName(u_int16_t typeIndex);


REGISTER_INCOMING_PDU(PDU_Lounge_CreditCardEdit, CpduCreditCardEdit);


CpduCreditCardEdit::CpduCreditCardEdit()
{}


void CpduCreditCardEdit::grok(SOCKET sd, CPlayer* player)
{
  if (player)
  {
    // Extract credit card data from buffer into container

	char* buf = (value_ + PDU_HEADERSIZE + sizeof(u_int16_t));
    CreditCards creditCards;

    string cctype, ccnumber, ccexpmo, ccexpyr;

    int numCards = getNumCreditCards();
    for (int i = 0; i < numCards; i++)
    {
      pdu_ccdata ccdata;
      memcpy(&ccdata, buf, sizeof(ccdata));
      ccdata.typeIndex_ = ntohs(ccdata.typeIndex_);

      cctype = GetCCTypeName(ccdata.typeIndex_);
      if (cctype.size() > 0)
      {
        ccnumber.resize(16);
        ccexpmo.resize(2);
        ccexpyr.resize(4);

	int j = 0;
        for (j = 0; j < 16; j++)
        {
          ccnumber[j] = ccdata.digits_[j];
        }

        for (j = 0; j < 2; j++)
        {
          ccexpmo[j] = ccdata.expmo_[j];
        }

        ccexpyr[0] = '2';
        ccexpyr[1] = '0';
        for (j = 0; j < 2; j++)
        {
          ccexpyr[2+j] = ccdata.expyr_[j];
        }
      }

      creditCards.push_back(CreditCardData(cctype.c_str(),
                                           ccnumber.c_str(),
                                           ccexpmo.c_str(),
                                           ccexpyr.c_str()));

      buf += sizeof(pdu_ccdata);
    }

    CdbInterface::Inst()->saveCreditCardData(
                            player->getUsername(),
                            creditCards);
                                         
  }
                                                  
  // this pdu is done
  delete this;
}
