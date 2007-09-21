#ifndef __lounge_pdusitdown_h__
#define __lounge_pdusitdown_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduSitdown
  Created  : 04/18/2000

  OVERVIEW : Lounge sitdown PDU - player wants to join a waiting list
             and subsequently sit down a table.

 ****************************************************************************/

#include "pdu.h"


struct pdu_sitdown
{
    u_int16_t queuenumber;
};

class CpduSitdown : public Cpdu
{
public:
    CpduSitdown();
    void grok(SOCKET, CPlayer*);

    u_int16_t getQueueNumber() const;
};

inline u_int16_t CpduSitdown::getQueueNumber() const
{
	pdu_sitdown* pdu = (pdu_sitdown*)(value_ + PDU_HEADERSIZE);
	return ntohs(pdu->queuenumber);
};


#endif
