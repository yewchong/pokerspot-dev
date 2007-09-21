/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTransactionResponse
  Created  : 05/26/2000

  OVERVIEW : Send-only Transaction Response.

 ****************************************************************************/

#include "pdutransactionresponse.h"

CpduTransactionResponse::CpduTransactionResponse()
{
    type_ = PDU_Lounge_TransactionReply;
    length_ = PDU_HEADER_SIZE;
    stuffHeader();
}

int CpduTransactionResponse::sendResponse(SOCKET sd,
                                          const char* msg)
{
    char* buf = value_ + PDU_HEADER_SIZE;

    if (msg != NULL && *msg != '\0')
    {
        size_t length = strlen(msg);
        memcpy(buf, msg, length);
        buf[length] = '\0';
        length_ += (length + 1);
    }

    stuffHeader();

    return sendTo(sd);
}

