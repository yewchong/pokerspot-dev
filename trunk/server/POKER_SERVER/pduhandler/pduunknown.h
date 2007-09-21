#ifndef __psi_pduunknown_h__
#define __psi_pduunknown_h__

#include "../pduhandler/pduhandler.h"

class CpduUnknown : public Cpdu
{
public:
    CpduUnknown();
    void grok(SOCKET, CTable*);
};


#endif