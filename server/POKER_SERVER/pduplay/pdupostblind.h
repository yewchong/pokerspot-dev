#ifndef __table_pdupostblind_h__
#define __table_pdupostblind_h__

#include "../pduhandler/pduhandler.h"

class CpduPostBlind : public Cpdu
{
public:
    CpduPostBlind();

// Note: Cpdu's default grok is enough
// for this pdu so there is no need to
// override it.
};


#endif