/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduTableLogin
  Created  : 04/21/2000

  OVERVIEW : Send-only table login PDU - Table server 
             sends this pdu as a "hello" message to the
             Lounge server.

 ****************************************************************************/

#include "pdutablelogin.h"
#include "common/chips.h"

struct pdu_tablelogin
{
  u_int16_t gametype;
  u_int16_t tablenumber;
  CChips::chip_amount_t tablehigh;
  CChips::chip_amount_t tablelow;
  u_int16_t tableport;
};

CpduTableLogin::CpduTableLogin()
{
  type_ = PDU_Server_TableLogin;
  length_ = PDU_HEADER_SIZE + sizeof(pdu_tablelogin);
  stuffHeader();
}

int CpduTableLogin::sendTableLogin(SOCKET sd,
                                   u_int16_t gametype,
                                   u_int16_t tablenumber,
                                   const CChips& tablelow,
                                   const CChips& tablehigh,
                                   u_int16_t tableport)
{
  pdu_tablelogin pdu;
  memset(&pdu, 0, sizeof(pdu));
  
  pdu.gametype = htons(gametype);
  pdu.tablenumber = htons(tablenumber),
  pdu.tablehigh = tablehigh.htonl_rep();
  pdu.tablelow = tablelow.htonl_rep();
  pdu.tableport = htons(tableport);
  
  memcpy(value_ + PDU_HEADER_SIZE, &pdu, sizeof(pdu));
  
  return sendTo(sd);
}
