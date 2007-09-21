#ifndef __lounge_pducreditcardedit_h__
#define __lounge_pducreditcardedit_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduCreditCardEdit
  Created  : 04/24/2001

  OVERVIEW : Client specifies the player's credit cards to the
             server with this pdu. Used to implement editing of
             credit cards within the client.

 ****************************************************************************/

#include "pdu.h"


class CpduCreditCardEdit : public Cpdu
{
public:
    CpduCreditCardEdit();
    void grok(SOCKET, CPlayer*);

    u_int16_t getNumCreditCards() const;
};

inline u_int16_t CpduCreditCardEdit::getNumCreditCards() const
{
	u_int16_t* data = (u_int16_t*)(value_ + PDU_HEADERSIZE);
	return ntohs(*data);
};


#endif
