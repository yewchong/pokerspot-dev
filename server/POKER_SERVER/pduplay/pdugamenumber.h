#ifndef __table_pdugamenumber_h__
#define __table_pdugamenumber_h__

#include "../pduhandler/pduhandler.h"

class CTable;

class CpduGameNumber : public Cpdu
{
public:
    CpduGameNumber();
    static int SendGameNumber(CTable* table);
    int sendGameNumber(CTable* table, u_int32_t number);
};


#endif