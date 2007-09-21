#ifndef __table_pduping_h__
#define __table_pduping_h__

#include "../pduhandler/pduhandler.h"

class CPlayer;

// Ping time allowed in seconds
#define PING_ACCEPT_THRESHOLD 6

// Number of ping failures allowed
#define PING_MAX_FAILURES 4


struct pdu_ping
{
    u_int32_t data;
};

class CpduPing : public Cpdu
{
public:
    CpduPing();
    int sendPing(CPlayer* player);
};


#endif