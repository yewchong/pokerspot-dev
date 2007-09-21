/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduObserving
  Created  : 05/27/2000

  OVERVIEW : Send-only observing PDU; this PDU tells the client that
             the player has been accepted to observe a table.

 ****************************************************************************/

#include "pduobserving.h"
#include "lounge/lounge.h"


struct pdu_observing
{
  u_int16_t table;
  u_int16_t port;
  u_int32_t host;
};


CpduObserving::CpduObserving()
{
    type_ = PDU_Lounge_Observing;
    length_ = PDU_HEADER_SIZE + sizeof(pdu_observing);
    stuffHeader();
}

int CpduObserving::sendObserving(SOCKET sd,
                                 u_int16_t table,
                                 u_int16_t port,
                                 u_int32_t host)
{
    pdu_observing pdu;
    memset(&pdu, 0, sizeof(pdu));

    pdu.table = htons(table);
    pdu.port = htons(port);
    pdu.host = htonl(host);

    memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));

    return sendTo(sd);
}

