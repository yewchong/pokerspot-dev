/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardReply2
  Created  : 04/24/2001

  OVERVIEW : Server responds to client's credit card query with this
             pdu. Used to implement editing of credit cards within
             the client.

 ****************************************************************************/

#include "pducreditcardreply2.h"
#include "sys.h"

// In pducreditcardquery2.cpp
extern u_int16_t GetCCTypeIndex(const string& cctype);

CpduCreditCardReply2::CpduCreditCardReply2()
{
    type_ = PDU_Lounge_CreditCardReply2;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

int CpduCreditCardReply2::sendCards(SOCKET sd,
                                    const list<CreditCardData>& ccards)
{
    length_ += sizeof(u_int16_t);
    stuffHeader();

    char* buf = value_ + PDU_HEADER_SIZE;
    u_int16_t numCards = htons(ccards.size());
    memcpy(buf, &numCards, sizeof(numCards));
    buf += sizeof(numCards);

    int c = 0;
    for (list<CreditCardData>::const_iterator i = ccards.begin(),
                                              e = ccards.end();
         i != e;
         ++i, ++c)
    {
        if (length_ + sizeof(pdu_ccdata) >= MAXBUFFERSIZE)
        {
            Sys_LogError("CpduCreditCardReply2: buffer size too small!\n");
            break;
        }

        const CreditCardData& card = *i;
        pdu_ccdata data;
        memset(&data, 0, sizeof(data));

        data.typeIndex_ = htons(GetCCTypeIndex(card.cctype_));

        // XXX FIXME: should pack the credit card number        
        for (int i = 0; i < 16; i++)
        {
          if (i >= card.ccnumber_.size())
            break;

          data.digits_[i] = card.ccnumber_[i];
        }

        // Expire month in two letters (e.g., 09)
        if (card.ccexpmo_.size() == 1)
        {
          data.expmo_[0] = '0';
          data.expmo_[1] = card.ccexpmo_[0];
        }
        else if (card.ccexpmo_.size() == 2)
        {
          data.expmo_[0] = card.ccexpmo_[0];
          data.expmo_[1] = card.ccexpmo_[1];
        }

        // Two last digits of the expire year (e.g., 2003 => 03)
        if (card.ccexpyr_.size() == 4)
        {
          data.expyr_[0] = card.ccexpyr_[2];
          data.expyr_[1] = card.ccexpyr_[3];
        }

        size_t s = sizeof(pdu_ccdata);
        memcpy(buf, &data, sizeof(pdu_ccdata));
        buf += sizeof(pdu_ccdata);

        length_ += sizeof(pdu_ccdata);
    }

    stuffHeader();

    return sendTo(sd);
}


