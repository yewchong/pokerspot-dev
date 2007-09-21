#ifndef __table_pduhearbeat_h__
#define __table_pduheartbeat_h__


#include "../pduhandler/pduhandler.h"


class CpduHeartBeat : public Cpdu
{
public:
    CpduHeartBeat();
    void grok(SOCKET, CTable*);
};


#endif