#ifndef __lounge_pduobserve_h__
#define __lounge_pduobserve_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduObserve
  Created  : 05/26/2000

  OVERVIEW : Lounge observe PDU - player wants to observe a table.

 ****************************************************************************/

#include "pdu.h"


struct pdu_observe
{
    u_int16_t tablenumber;
};

class CpduObserve : public Cpdu
{
public:
    CpduObserve();
    void grok(SOCKET, CPlayer*);

    u_int16_t getTableNumber() const;
};

inline u_int16_t CpduObserve::getTableNumber() const
{
	pdu_observe* pdu = (pdu_observe*)(value_ + PDU_HEADERSIZE);
	return ntohs(pdu->tablenumber);
};


#endif
