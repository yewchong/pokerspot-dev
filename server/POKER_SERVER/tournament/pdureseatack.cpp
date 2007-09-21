/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduReseatAck
  Created  : 07/25/2000

  OVERVIEW : Send-only reseat ack pdu.
             Table server tells lounge server whether seating a player
             was successfull or not.

 ****************************************************************************/

#include "tournament/pdureseatack.h"

CpduReseatAck::CpduReseatAck()
{
    type_ = PDU_Tournament_ReseatAck;
    length_ = PDU_HEADERSIZE + sizeof(pdu_reseatack);
    stuffHeader();
}

int CpduReseatAck::sendAck(SOCKET      sd,
                           const char* username)
{
    type_ = PDU_Tournament_ReseatAck;
    length_ = PDU_HEADERSIZE + sizeof(pdu_reseatack);
    stuffHeader();

    pdu_reseatack pdu;
    memset(&pdu, 0, sizeof(pdu));

    strncpy(pdu.username, username, PDU_STRINGSIZE);

    memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}

int CpduReseatAck::sendNack(SOCKET      sd,
                           const char* username)
{
    type_ = PDU_Tournament_ReseatNack;
    length_ = PDU_HEADERSIZE + sizeof(pdu_reseatack);
    stuffHeader();

    pdu_reseatack pdu;
    memset(&pdu, 0, sizeof(pdu));

    strncpy(pdu.username, username, PDU_STRINGSIZE);

    memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}
