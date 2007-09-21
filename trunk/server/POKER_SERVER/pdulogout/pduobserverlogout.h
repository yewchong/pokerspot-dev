#ifndef __table_pduobserverlogout_h__
#define __table_pduobserverlogout_h__

#include "../pduhandler/pduhandler.h"

class CpduObserverLogout : public Cpdu
{
public:
    CpduObserverLogout();
    void grok(SOCKET, CTable*);
};


#endif
