#ifndef __table_pdushowdown_h__
#define __table_pdushowdown_h__

/*******************************************************************************

    NAME:       pduShowdown
    DATE:       4/16/00
    PURPOSE:    Send-only Showdown pdu

*******************************************************************************/


#include "../pduhandler/pduhandler.h"


class CpduShowdown : public Cpdu
{
public:
    CpduShowdown(CTable*);
    int sendShowdown(CPlayer* player,
                     int numCards,
                     const char* message);

private:
    CTable* table_;
};


#endif