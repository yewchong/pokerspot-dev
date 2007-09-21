#ifndef __table_pdusitin_h__
#define __table_pdusitin_h__

#include "../pduhandler/pduhandler.h"

class CpduSitIn : public Cpdu
{
public:
    CpduSitIn();

    void grok(SOCKET, CTable*);
};

#endif