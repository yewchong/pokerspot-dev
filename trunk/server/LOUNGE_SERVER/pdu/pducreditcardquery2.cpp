/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardQuery2
  Created  : 04/24/2001

  OVERVIEW : Client queries the player's credit cards with this
             pdu. Used to implement editing of credit cards within
             the client.

 ****************************************************************************/

#include "pdu/pducreditcardquery2.h"
#include "pdu/pducreditcardreply2.h"
#include "pdu/pducreditcardtypes.h"
#include "dbhandler/dbinterface.h"
#include "lounge/player.h"

REGISTER_INCOMING_PDU(PDU_Lounge_CreditCardQuery2, CpduCreditCardQuery2);


u_int16_t GetCCTypeIndex(const string& cctype)
{
  if (cctype == "visa")
    return 0;
  else if (cctype == "mastercard")
    return 1;
  else
    return 0;
}

string GetCCTypeName(u_int16_t typeIndex)
{
  if (typeIndex == 0)
    return "visa";
  else if (typeIndex == 1)
    return "mastercard";
  else
    return "";
}


CpduCreditCardQuery2::CpduCreditCardQuery2()
{}


void CpduCreditCardQuery2::grok(SOCKET sd, CPlayer* player)
{
  if (player)
  {
    if (getTypes() == 1)
    {
      CpduCreditCardTypes pdu;

      list<CreditCardType> cctypes;
      // For now, visa and mastercard supported
      cctypes.push_back("visa");
      cctypes.push_back("mastercard");
      pdu.sendTypes(sd, cctypes);
    }

    CreditCards creditCards;
    if (CdbInterface::Inst()->getCreditCardData(player->getUsername(),
                                                creditCards))
    {
      CpduCreditCardReply2 pdu;
      pdu.sendCards(sd, creditCards);
    }
  }
                                                  
  // this pdu is done
  delete this;
}
