#ifndef __lounge_pducreditcardquery2_h__
#define __lounge_pducreditcardquery2_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardQuery2
  Created  : 04/24/2001

  OVERVIEW : Client queries the player's credit cards with this
             pdu. Used to implement editing of credit cards within
             the client.

 ****************************************************************************/

#include "pdu.h"


struct pdu_qdata
{
    u_int16_t types;
};

class CpduCreditCardQuery2 : public Cpdu
{
public:
    CpduCreditCardQuery2();
    void grok(SOCKET, CPlayer*);

    u_int16_t getTypes() const;
};

inline u_int16_t CpduCreditCardQuery2::getTypes() const
{
	pdu_qdata* pdu = (pdu_qdata*)(value_ + PDU_HEADERSIZE);
	return ntohs(pdu->types);
};


#endif
