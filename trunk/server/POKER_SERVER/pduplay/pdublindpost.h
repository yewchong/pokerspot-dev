#ifndef __table_pdupostblind_h__
#define __table_pdupostblind_h__

/****************************************************************************

  Copyright (c) PokerSpot International, 2000

  Title    : class CpduBlindPost
  Created  : 08/13/2000

  OVERVIEW : The table client sends this PDU to the server to tell
             that the player will post the blind.

 ****************************************************************************/


#include "pduhandler/pduhandler.h"

class CpduPostBlind : public Cpdu
{
public:
  CpduPostBlind();

// Note: Cpdu's default grok which places the
// pdu to player's queue is enough, so there
// is no need to override it.

  u_int16_t getSequenceNumber() const;
};

inline u_int16_t CpduPostBlind::getSequenceNumber() const
{
	u_int16_t* numptr = (u_int16_t*)(value_ + PDU_HEADERSIZE);
	return ntohs(*numptr);
}

#endif