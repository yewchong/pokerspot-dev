#ifndef __table_pdubuyinrequest_h__
#define __table_pdubuyinrequest_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2001

  Title    : class CpduBuyinRequest
  Created  : 1/1/2001

  OVERVIEW : This class is used to implement player's buy-in
             when he sits at the table.

 ****************************************************************************/



#include "pduhandler/pduhandler.h"
#include "common/chips.h"

typedef struct pdu_buyinrequest pdu_buyinrequest_t;

struct pdu_buyinrequest
{
	CChips::chip_amount_t chips;
};

class CpduBuyinRequest : public Cpdu
{
public:
  CpduBuyinRequest();
	CpduBuyinRequest(Cpdu*);
	CChips getChips() const;
  void grok(SOCKET, CTable*);
};


inline CChips CpduBuyinRequest::getChips() const
{
	pdu_buyinrequest_t* pBuyinR = (pdu_buyinrequest_t*)(value_ + PDU_HEADERSIZE);
  CChips_n chips_n(pBuyinR->chips);
	return chips_n.ntohl();
};


#endif 
