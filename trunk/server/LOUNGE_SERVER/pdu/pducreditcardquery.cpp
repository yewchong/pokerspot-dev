/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardQuery
  Created  : 04/19/2000

  OVERVIEW : Receive-only credit card query PDU - player wants to know the
             credit cards.

 ****************************************************************************/

#include "pducreditcardquery.h"
#include "pducreditcardreply.h"
#include "dbhandler/dbinterface.h"

REGISTER_INCOMING_PDU(PDU_Lounge_CreditCardQuery, CpduCreditCardQuery);

CpduCreditCardQuery::CpduCreditCardQuery()
{}

void CpduCreditCardQuery::grok(SOCKET sd, CPlayer* player)
{
    list<pdu_creditcard> creditCards;
    if (CdbInterface::Inst()->getCreditCards(player, creditCards))
    {
        CpduCreditCardReply pdu;
        pdu.sendCards(creditCards, player);
    }

    // this pdu is done
    delete this;
}
