/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduBuyinQuery
  Created  : 1/1/2001

  OVERVIEW : This class is used to send a buy-in query to a player.

 ****************************************************************************/

#include "pdubuyinquery.h"

CpduBuyinQuery::CpduBuyinQuery(CTable* table)
{
	type_ = PDU_Setup_BuyInQuery;
	length_ = PDU_HEADERSIZE + sizeof(pdu_buyinquery_t);
	table_ = table;
}

int CpduBuyinQuery::sendBuyinQuery(CPlayer* player)
{
	pdu_buyinquery_t buyinQuery;
  CdbInterface* db = table_->getDbase();
    
  CChips chips;
  db->getChips(player, chips);

	buyinQuery.chips = chips.htonl_rep();

	stuffHeader();

	memcpy(value_ + PDU_HEADERSIZE, &buyinQuery, sizeof(pdu_buyinquery_t));

	return sendTo(player->getConnfd());
}