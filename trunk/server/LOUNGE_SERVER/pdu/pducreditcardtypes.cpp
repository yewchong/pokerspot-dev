/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardTypes
  Created  : 04/24/2001

  OVERVIEW : Server responds to client's credit card query with this
             pdu if the client want to know the supported credit card
             types. Used to implement editing of credit cards within
             the client.

 ****************************************************************************/

#include "pducreditcardtypes.h"
#include "sys.h"

// Function defined in pdustatus.cpp
extern int SerializeString(char* buf, const char* s);

CpduCreditCardTypes::CpduCreditCardTypes()
{
    type_ = PDU_Lounge_CreditCardTypes;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

int CpduCreditCardTypes::sendTypes(SOCKET sd,
                                   const list<CreditCardType>& cctypes)
{
    char* buf = value_ + PDU_HEADER_SIZE;

    // Copy number of credit card types to buffer
    u_int16_t numTypes = cctypes.size();
    u_int16_t* pint = (u_int16_t*)buf;
    *pint++ = htons(numTypes);
    length_ += sizeof(numTypes);
    
    // Copy credit card type strings to buffer
    for (list<CreditCardType>::const_iterator it = cctypes.begin(),
                                              end = cctypes.end();
         it != end;
         ++it)
    {
        CreditCardType cctype = *it;

        // Put credit card type name string to buffer
        buf = (char*)pint;
        const char* n = cctype.cctype_.c_str();

        int l = n ? strlen(n) : 0;
        if (length_ + l >= MAXBUFFERSIZE)
        {
            Sys_LogError("CpduCreditCardTypes: buffer size too small for game names!\n");
            break;
        }

        int nameLength = SerializeString(buf, n);
        buf += nameLength;
        length_ += nameLength;

        pint = (u_int16_t*)buf;
    }

    stuffHeader();

    return sendTo(sd);
}

