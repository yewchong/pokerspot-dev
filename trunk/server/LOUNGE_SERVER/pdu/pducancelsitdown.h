#ifndef __lounge_pducancelsitdown_h__
#define __lounge_pducancelsitdown_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCancelSitdown
  Created  : 04/18/2000

  OVERVIEW : Lounge cancel sitdown PDU - player wants to be removed
             from a waiting list.

 ****************************************************************************/

#include "pdu.h"


struct pdu_cancelsitdown
{
    u_int16_t queuenumber;
};

class CpduCancelSitdown : public Cpdu
{
public:
    CpduCancelSitdown();
    void grok(SOCKET, CPlayer*);

    u_int16_t getQueueNumber() const;
};

inline u_int16_t CpduCancelSitdown::getQueueNumber() const
{
	pdu_cancelsitdown* pdu = (pdu_cancelsitdown*)(value_ + PDU_HEADERSIZE);
	return ntohs(pdu->queuenumber);
};


#endif
