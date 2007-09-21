#ifndef __table_pdurefuseblind_h__
#define __table_pdurefuseblind_h__

#include "../pduhandler/pduhandler.h"

class CpduRefuseBlind : public Cpdu
{
public:
    CpduRefuseBlind();

// Note: Cpdu's default grok which places the
// pdu to player's queue is enough, so there
// is no need to override it.

    u_int16_t getSequenceNumber() const;

};

inline u_int16_t CpduRefuseBlind::getSequenceNumber() const
{
	u_int16_t* numptr = (u_int16_t*)(value_ + PDU_HEADERSIZE);
	return ntohs(*numptr);
}

#endif