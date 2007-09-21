#ifndef __table_pdubuyinquery_h__
#define __table_pdubuyinquery_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduBuyinQuery
  Created  : 1/1/2001

  OVERVIEW : This class is used to send a buy-in query to a player.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"
#include "dbinterface/dbinterface.h"
#include "table/table.h"
#include "player/player.h"
#include "common/chips.h"


typedef struct pdu_buyinquery pdu_buyinquery_t;

struct pdu_buyinquery
{
	CChips::chip_amount_t chips;
};


class CpduBuyinQuery : public Cpdu
{
public:
	CpduBuyinQuery(CTable*);
	int sendBuyinQuery(CPlayer*);

private:
	CTable* table_;
};


#endif

