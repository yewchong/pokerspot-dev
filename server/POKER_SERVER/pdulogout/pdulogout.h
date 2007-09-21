#ifndef __table_pdulogout_h__
#define __table_pdulogout_h__

#include "../pduhandler/pduhandler.h"


class CpduLogout : public Cpdu
{
public:
    CpduLogout();
	CpduLogout(Cpdu *);
    void grok(SOCKET, CTable*);
};


#endif
