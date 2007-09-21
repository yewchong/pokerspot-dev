#ifndef __table_pdupong_h__
#define __table_pdupong_h__


#include "../pduhandler/pduhandler.h"


class CpduPong : public Cpdu
{
public:
    CpduPong();
    void grok(SOCKET, CTable*);
};


#endif