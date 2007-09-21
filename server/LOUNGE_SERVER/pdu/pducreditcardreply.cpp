/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardReply
  Created  : 04/19/2000

  OVERVIEW : Send-only credit card reply PDU.

 ****************************************************************************/

#include "pducreditcardreply.h"
#include "lounge/player.h"
#include "dbhandler/dbinterface.h"
#include "sys.h"

CpduCreditCardReply::CpduCreditCardReply()
{
    type_ = PDU_Lounge_CreditCardReply;
    length_ = PDU_HEADER_SIZE + sizeof(u_int16_t);
    stuffHeader();
}

int CpduCreditCardReply::sendCards(const list<pdu_creditcard>& cards,
                                   CPlayer* player)
{
    length_ += cards.size() * sizeof(pdu_creditcard);
    stuffHeader();

    char* buf = value_ + PDU_HEADER_SIZE;
    u_int16_t numCards = htons(cards.size());
    memcpy(buf, &numCards, sizeof(numCards));
    buf += sizeof(numCards);

    int c = 0;
    for (list<pdu_creditcard>::const_iterator i = cards.begin(),
                                              e = cards.end();
         i != e;
         ++i, ++c)
    {
        if (length_ + (c + 1) * sizeof(pdu_creditcard) >= MAXBUFFERSIZE)
        {
            Sys_LogError("CpduCreditCardReply: buffer size too small!\n");
            break;
        }

        const pdu_creditcard& card = *i;
        memcpy(buf, &card, sizeof(card));
        buf += sizeof(card);
    }

    return sendTo(player->getSocket());
}

