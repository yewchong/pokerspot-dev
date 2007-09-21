#ifndef __table_pdusitinquery_h__
#define __table_pdusitinquery_h__

#include "../pduhandler/pduhandler.h"

class CTable;
class CPlayer;

class CpduSitInQuery : public Cpdu
{
public:
    CpduSitInQuery(CTable* table);
    int sendSitInQuery(CPlayer*);
private:
    CTable* table_;
};


#endif