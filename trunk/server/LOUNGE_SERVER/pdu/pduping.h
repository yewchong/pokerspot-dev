#ifndef __lounge_pduping_h__
#define __lounge_pduping_h__

#include "pdu.h"

struct pdu_ping
{
    u_int32_t data;
};

class CpduPing : public Cpdu
{
public:
    CpduPing();
    int sendPing(CPlayer* player, long now);
};


#endif