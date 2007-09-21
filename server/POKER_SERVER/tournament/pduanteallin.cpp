/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduAnteAllIn
  Created  : 08/13/2000

  OVERVIEW : This pdu is used to prepare the client to do an
             all-in on subsequent ante.

 ****************************************************************************/

#include "tournament/pduanteallin.h"
#include "table/table.h"
#include "common/chips.h"


struct pdu_anteallin
{
  CChips::chip_amount_t bet;
};


CpduAnteAllIn::CpduAnteAllIn()
{
  type_ = PDU_Tournament_AnteAllIn;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_anteallin);
  stuffHeader();
}

int CpduAnteAllIn::sendAllIn(CTable* table,
                             const CChips& bet)
{
  pdu_anteallin pdu;
  memset(&pdu, 0, sizeof(pdu));

  pdu.bet = bet.htonl_rep();

  memcpy(value_ + PDU_HEADERSIZE, &pdu, sizeof(pdu));

  return broadcastTo(table->getRing());
}

